/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * Everything concerning artifacts.
 * @see page_treasure_list
 */

#include "global.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "loader.h"

/**
 * Allocate and return the pointer to an empty artifactlist structure.
 *
 * @return
 * new structure blanked, never NULL.
 *
 * @note
 * will fatal() if memory error.
 */
artifactlist *get_empty_artifactlist(void) {
    artifactlist *tl = new artifactlist();
    if (tl == NULL)
        fatal(OUT_OF_MEMORY);
    tl->next = NULL;
    tl->total_chance = 0;
    return tl;
}

/**
 * Allocate and return the pointer to an empty artifact structure.
 *
 * @return
 * new structure blanked, never NULL.
 *
 * @note
 * will fatal() if memory error.
 */
artifact *get_empty_artifact(void) {
    artifact *t = new artifact();
    if (t == NULL)
        fatal(OUT_OF_MEMORY);
    t->item = NULL;
    t->chance = 0;
    t->difficulty = 0;
    return t;
}

/**
 * Totally frees an artifact, its next items, and such.
 *
 * @param at
 * artifact to free. Pointer is free()d too, so becomes invalid.
 *
 * @note
 * Objects at->item are malloc()ed by init_artifacts(), so can be free()d.
 */
static void free_artifact(artifact *at) {
    object *next;

    for (auto allowed : at->allowed)
        free_string(allowed);
    while (at->item) {
        next = at->item->next;
        if (at->item->name)
            free_string(at->item->name);
        if (at->item->name_pl)
            free_string(at->item->name_pl);
        if (at->item->msg)
            free_string(at->item->msg);
        if (at->item->title)
            free_string(at->item->title);
        object_free_key_values(at->item);
        free(at->item);
        at->item = next;
    }
    delete at;
}

/**
 * Free specified list and its items.
 *
 * @param al
 * list to free. Pointer is free()d too, so becomes invalid.
 */
static void free_artifactlist(artifactlist *al) {
    artifactlist *nextal;

    for (; al != NULL; al = nextal) {
        nextal = al->next;
        for (auto art : al->items) {
            free_artifact(art);
        }
        delete al;
    }
}

/**
 * Free all artifact-related information.
 */
void free_all_artifacts(void) {
    free_artifactlist(first_artifactlist);
    first_artifactlist = NULL;
}

/** Give 1 re-roll attempt per artifact */
#define ARTIFACT_TRIES 2

/**
 * Compute the chance for a specified item to become the specified artifact.
 * This does not take into account the 10% chance of an item being made an artifact.
 * @param op item to consider.
 * @param art artifact to compute the chance of.
 * @param numerator chance in denominator for the artifact to be generated.
 * @param denominator denominator, will never be 0.
 */
void artifact_compute_chance_for_item(const object *op, const artifact *art, int *numerator, int *denominator) {
    (*numerator) = 0;
    (*denominator) = 1;
    const artifactlist *list = find_artifactlist(op->type);
    if (!list || list->total_chance == 0) {
        return;
    }

    int chance_of_invalid_item = 0;
    for (auto check : list->items) {
        if (!legal_artifact_combination(op, check)) {
            chance_of_invalid_item += check->chance;
        }
    }

    /*
        Let:
         - 'ac' be the artifact's chance as given in the list (art->chance)
         - 'ic' the chance to find an illegal artifact in the list (chance_of_invalid_item)
         - 'tc' the total chance of the list (list->total_chance)

        The chance of the artifact being generated at the first try is ac/tc
        The chance of finding an invalid artifact is ic/tc
        So the chance of generating an item on the second try is (ic/tc) * (ac/tc)
        The chance of the artifact being generated is thus (ac/tc) + (ic/tc) * (ac/tc)

        In numerator/denominator notation, this gives ac * (tc + ic) / (tc²)
    */

    (*numerator) = art->chance * (list->total_chance + chance_of_invalid_item);
    (*denominator) = list->total_chance * list->total_chance;
}

/**
 * Decides randomly which artifact the object should be
 * turned into.  Makes sure that the item can become that
 * artifact (means magic, difficulty, and Allowed fields properly).
 * Then calls give_artifact_abilities in order to actually create
 * the artifact.
 *
 * @param op object to attempt to transform.
 * @param difficulty limit for artifact difficulty.
 * @note if the algorithm is changed, please update @ref artifact_compute_chance_for_item above.
 */
void generate_artifact(object *op, int difficulty) {
    const artifactlist *al;
    const artifact *art = NULL;
    int i;

    al = find_artifactlist(op->type);

    if (al == NULL) {
        return;
    }

    for (i = 0; i < ARTIFACT_TRIES; i++) {
        int roll = RANDOM()%al->total_chance;

        for (auto r : al->items) {
            roll -= r->chance;
            if (roll < 0) {
                art = r;
                break;
            }
        }

        if (art == NULL || roll >= 0) {
            LOG(llevError, "Got null entry and non zero roll in generate_artifact, type %d\n", op->type);
            return;
        }
        if (!strcmp(art->item->name, "NONE"))
            return;
        if (FABS(op->magic) < art->item->magic)
            continue; /* Not magic enough to be this item */

        /* Map difficulty not high enough */
        if (difficulty < art->difficulty)
            continue;

        if (!legal_artifact_combination(op, art)) {
#ifdef TREASURE_VERBOSE
            LOG(llevDebug, "%s of %s was not a legal combination.\n", op->name, art->item->name);
#endif
            continue;
        }
        give_artifact_abilities(op, art->item);
        return;
    }
}

/**
 * Fixes the given object, giving it the abilities and titles
 * it should have due to the second artifact-template.
 *
 * @param op object to give properties to.
 * @param artifact what properties to apply.
 */
void give_artifact_abilities(object *op, const object *artifact) {
    char new_name[MAX_BUF];

    snprintf(new_name, sizeof(new_name), "of %s", artifact->name);
    if (op->title)
        free_string(op->title);
    op->title = add_string(new_name);
    if (op->artifact)
        free_string(op->artifact);
    op->artifact = add_refcount(artifact->name);
    add_abilities(op, artifact); /* Give out the bonuses */

    return;
}

/**
 * Checks if op can be combined with art.
 *
 * @param op object to test.
 * @param art artifact to test.
 * @return 1 if 'op' can be 'art', 0 else.
 */
int legal_artifact_combination(const object *op, const artifact *art) {
    int neg, success = 0;
    const char *name;

    if (art->allowed.empty())
        return 1; /* Ie, "all" */
    for (auto tmp :  art->allowed) {
#ifdef TREASURE_VERBOSE
        LOG(llevDebug, "legal_art: %s\n", tmp->name);
#endif
        if (*tmp == '!') {
            name = tmp+1;
            neg = 1;
        } else {
            name = tmp;
            neg = 0;
        }

        /* If we match name, then return the opposite of 'neg' */
        if (op->name && (!strcmp(name, op->name) || (op->arch && !strcmp(name, op->arch->name))))
            return !neg;

        /* Set success as true, since if the match was an inverse, it means
         * everything is allowed except what we match
         */
        else if (neg)
            success = 1;
    }
    return success;
}

/**
 * Compute the name of a face with a suffix, taking into account names like '.123' or '.1xx'.
 * @param buf where to put the resulting name.
 * @param size length of buf.
 * @param name base face name.
 * @param suffix suffix to add to the face name.
 */
static void compute_face_name(char* buf, size_t size, const char* name, const char* suffix)
{
    const char* dot = name + strlen(name) - 1;
    while (dot > name && (isdigit(*dot) || (*dot == 'x')))
    {
      dot--;
    }

    if (*dot == '.')
    {
        buf[0] = '0';
        strlcpy(buf, name, dot - name + 1);
        snprintf(buf + strlen(buf), size - strlen(buf), "_%s%s", suffix, dot);
    }
    else
    {
        snprintf(buf, size, "%s_%s", name, suffix);
    }
}

/* Keys used for artifact stuff, not copied */
#define KEY_FACE_SUFFIX         "face_suffix"
#define KEY_ANIMATION_SUFFIX    "animation_suffix"

/**
 * Apply artifact properties to an object.
 *
 * @param op object to apply changes to.
 * @param change changes to apply, with fields handled as described in the artifacts file.
 */
void add_abilities(object *op, const object *change) {
    int i, tmp;
    char buf[MAX_BUF];
    sstring key;

    if (change->face != NULL && change->face != blank_face) {
#ifdef TREASURE_VERBOSE
        LOG(llevDebug, "FACE: %d\n", change->face->number);
#endif

        object_set_value(op, "identified_face", change->face->name, 1);
    } else if ((key = object_get_value(change, KEY_FACE_SUFFIX)) != NULL) {
        const Face* face;
        compute_face_name(buf, sizeof(buf), op->face->name, key);
        face = try_find_face(buf, op->face);
        object_set_value(op, "identified_face", face->name, 1);
    }
    if (QUERY_FLAG(change, FLAG_CLIENT_ANIM_RANDOM)) {
        object_set_value(op, "identified_anim_random", "1", 1);
    }
    if (change->anim_speed > 0) {
        snprintf(buf, sizeof(buf), "%d", change->anim_speed);
        object_set_value(op, "identified_anim_speed", buf, 1);
    }
    if (change->animation != NULL && op->arch != NULL) {
        /* op->arch can be NULL when called from artifact_msg(). */
        object_set_value(op, "identified_animation", change->animation->name, 1);
    } else if (op->animation != NULL && (key = object_get_value(change, KEY_ANIMATION_SUFFIX)) != NULL) {
        const Animations *anim;
        snprintf(buf, sizeof(buf), "%s_%s", op->animation->name, key);
        anim = try_find_animation(buf);
        if (anim != NULL) {
            object_set_value(op, "identified_animation", anim->name, 1);
        }
    }

    /**
     * For items that do not need identifying, use their identified face/animation
     * immediately. This should make seas of generated creatures look more interesting.
     *
     * Neila Hawkins
     * 2018-01-07
     *
     * Add query flags to perform the same treatment for identified items or artifacts that come identified.
     * Also make sure we have op->arch. Otherwise it segfaults when we don't.
     *
     * 2018-01-08
     */
    if (op->arch && (is_identified(op) || QUERY_FLAG(change, FLAG_IDENTIFIED)))
        object_give_identified_properties(op);

    for (i = 0; i < NUM_STATS; i++)
        change_attr_value(&(op->stats), i, get_attr_value(&(change->stats), i));

    op->attacktype |= change->attacktype;
    op->path_attuned |= change->path_attuned;
    op->path_repelled |= change->path_repelled;
    op->path_denied |= change->path_denied;
    op->move_type |= change->move_type;
    op->stats.luck += change->stats.luck;

    if (QUERY_FLAG(change, FLAG_CURSED))
        SET_FLAG(op, FLAG_CURSED);
    if (QUERY_FLAG(change, FLAG_DAMNED))
        SET_FLAG(op, FLAG_DAMNED);
    if ((QUERY_FLAG(change, FLAG_CURSED) || QUERY_FLAG(change, FLAG_DAMNED))
    && op->magic > 0)
        set_abs_magic(op, -op->magic);

    if (QUERY_FLAG(change, FLAG_LIFESAVE))
        SET_FLAG(op, FLAG_LIFESAVE);
    if (QUERY_FLAG(change, FLAG_REFL_SPELL))
        SET_FLAG(op, FLAG_REFL_SPELL);
    if (QUERY_FLAG(change, FLAG_STEALTH))
        SET_FLAG(op, FLAG_STEALTH);
    if (QUERY_FLAG(change, FLAG_XRAYS))
        SET_FLAG(op, FLAG_XRAYS);
    if (QUERY_FLAG(change, FLAG_BLIND))
        SET_FLAG(op, FLAG_BLIND);
    if (QUERY_FLAG(change, FLAG_CONFUSED))
        SET_FLAG(op, FLAG_CONFUSED);
    if (QUERY_FLAG(change, FLAG_SEE_IN_DARK))
        SET_FLAG(op, FLAG_SEE_IN_DARK);
    if (QUERY_FLAG(change, FLAG_REFL_MISSILE))
        SET_FLAG(op, FLAG_REFL_MISSILE);
    if (QUERY_FLAG(change, FLAG_MAKE_INVIS))
        SET_FLAG(op, FLAG_MAKE_INVIS);
    if (QUERY_FLAG(change, FLAG_REFLECTING))
        SET_FLAG(op, FLAG_REFLECTING);

    if (QUERY_FLAG(change, FLAG_STAND_STILL)) {
        CLEAR_FLAG(op, FLAG_ANIMATE);
        /* so artifacts will join */
        if (!QUERY_FLAG(op, FLAG_ALIVE))
            op->speed = 0.0;
        object_update_speed(op);
    }
    if (change->nrof != 0 && change->nrof != 1) {
        LOG(llevDebug, "archetype %s has nrof set to %d, which will be ignored\n",
                change->name, change->nrof);
    }
    op->stats.exp += change->stats.exp; /* Speed modifier */
    op->stats.wc += change->stats.wc;
    op->stats.ac += change->stats.ac;

    if (change->other_arch) {
        /* Basically, for horns & potions, the other_arch field is the spell
         * to cast.  So convert that to into a spell and put it into
         * this object.
         */
        if (op->type == ROD || op->type == POTION) {
            object *tmp_obj;

            /* Remove any spells this object currently has in it */
            while (op->inv) {
                tmp_obj = op->inv;
                object_remove(tmp_obj);
                object_free_drop_inventory(tmp_obj);
            }
            tmp_obj = arch_to_object(change->other_arch);
            /* This is an artifact, so this function will be called at load time,
             * thus we don't need to keep the inventory */
            SET_FLAG(tmp_obj, FLAG_NO_SAVE);
            object_insert_in_ob(tmp_obj, op);
        }
        /* No harm setting this for potions/horns */
        op->other_arch = change->other_arch;
    }

    if (change->stats.hp < 0)
        op->stats.hp = -change->stats.hp;
    else
        op->stats.hp += change->stats.hp;
    if (change->stats.maxhp < 0)
        op->stats.maxhp = -change->stats.maxhp;
    else
        op->stats.maxhp += change->stats.maxhp;
    if (change->stats.sp < 0)
        op->stats.sp = -change->stats.sp;
    else
        op->stats.sp += change->stats.sp;
    if (change->stats.maxsp < 0)
        op->stats.maxsp = -change->stats.maxsp;
    else
        op->stats.maxsp += change->stats.maxsp;
    if (change->stats.grace < 0)
        op->stats.grace = -change->stats.grace;
    else
        op->stats.grace += change->stats.grace;
    if (change->stats.maxgrace < 0)
        op->stats.maxgrace = -change->stats.maxgrace;
    else
        op->stats.maxgrace += change->stats.maxgrace;
    if (change->stats.food < 0)
        op->stats.food = -(change->stats.food);
    else
        op->stats.food += change->stats.food;
    if (change->level < 0)
        op->level = -(change->level);
    else
        op->level += change->level;

    op->item_power = change->item_power;

    for (i = 0; i < NROFATTACKS; i++) {
        if (change->resist[i]) {
            op->resist[i] += change->resist[i];
        }
    }
    if (change->stats.dam) {
        if (change->stats.dam < 0)
            op->stats.dam = (-change->stats.dam);
        else if (op->stats.dam) {
            tmp = (int)(((int)op->stats.dam*(int)change->stats.dam)/10);
            if (tmp == op->stats.dam) {
                if (change->stats.dam < 10)
                    op->stats.dam--;
                else
                    op->stats.dam++;
            } else
                op->stats.dam = tmp;
        }
    }
    if (change->weight) {
        if (change->weight < 0)
            op->weight = (-change->weight);
        else
            op->weight = (op->weight*(change->weight))/100;
    }
    if (change->last_sp) {
        if (change->last_sp < 0)
            op->last_sp = (-change->last_sp);
        else
            op->last_sp = (signed char)(((int)op->last_sp*(int)change->last_sp)/(int)100);
    }
    if (change->gen_sp_armour) {
        if (change->gen_sp_armour < 0)
            op->gen_sp_armour = (-change->gen_sp_armour);
        else
            op->gen_sp_armour = (signed char)(((int)op->gen_sp_armour*((int)change->gen_sp_armour))/(int)100);
    }
    op->value *= change->value;

    if (change->material)
        op->material = change->material;

    if (change->materialname) {
        if (op->materialname)
            free_string(op->materialname);
        op->materialname = add_refcount(change->materialname);
    }

    if (change->slaying) {
        if (op->slaying)
            free_string(op->slaying);
        op->slaying = add_refcount(change->slaying);
    }
    if (change->race) {
        if (op->race)
            free_string(op->race);
        op->race = add_refcount(change->race);
    }
    if (change->msg)
        object_set_msg(op, change->msg);

    if (change->key_values) {
        const key_value *kv = change->key_values;
        while (kv) {
            if (strcmp(kv->key, KEY_FACE_SUFFIX) != 0 && strcmp(kv->key, KEY_ANIMATION_SUFFIX) != 0) {
                object_set_value(op, kv->key, kv->value, 1);
            }
            kv = kv->next;
        }
    }

    if (change->inv) {
        object *copy;

        FOR_INV_PREPARE(change, inv) {
            copy = object_new();
            object_copy(inv, copy);
            SET_FLAG(copy, FLAG_NO_SAVE);
            object_insert_in_ob(copy, op);
        } FOR_INV_FINISH();
    }
}

/**
 * Finds the artifact list for a certain item type.
 *
 * @param type item type to get the artifacts of.
 * @return
 * NULL if no suitable list found.
 */
artifactlist *find_artifactlist(int type) {
    artifactlist *al;

    for (al = first_artifactlist; al != NULL; al = al->next)
        if (al->type == type)
            return al;
    return NULL;
}

/**
 * Searches and returns a specific artifact compatible with an object, NULL if not found.
 * @param op item to search the artifact for.
 * @param name artifact name.
 * @return matching artifact, NULL if none matched.
 */
const artifact *find_artifact(const object *op, const char *name) {
    artifactlist *list;
    sstring sname = find_string(name);

    if (sname == NULL)
        return NULL;

    list = find_artifactlist(op->type);
    if (list == NULL)
        return NULL;

    for (const auto at : list->items) {
        if (at->item->name == sname && legal_artifact_combination(op, at))
            return at;
    }

    return NULL;
}

/**
 * For debugging purposes.  Dumps all tables.
 *
 * @todo
 * use LOG() instead of fprintf.
 */
void dump_artifacts(void) {
    artifactlist *al;

    fprintf(logfile, "\n");
    for (al = first_artifactlist; al != NULL; al = al->next) {
        fprintf(logfile, "Artifact has type %d, total_chance=%d\n", al->type, al->total_chance);
        for (const auto art : al->items) {
            fprintf(logfile, "Artifact %-30s Difficulty %3d Chance %5d\n", art->item->name, art->difficulty, art->chance);
            if (!art->allowed.empty()) {
                fprintf(logfile, "\tAllowed combinations:");
                for (auto allowed : art->allowed)
                    fprintf(logfile, "%s,", allowed);
                fprintf(logfile, "\n");
            }
        }
    }
    fprintf(logfile, "\n");
}

/**
 * Get a suitable face number for representing an artifact.
 * @param art what to get the face of.
 * @return face, -1 as unsigned if none could be found.
 */
uint16_t artifact_get_face(const artifact *art) {
    if (art->item->face != blank_face && art->item->face != NULL)
        return art->item->face->number;

    archetype *arch = get_next_archetype(NULL);

    if (!art->allowed.empty()) {
        if (*art->allowed[0] == '!') {
            while (arch) {
                if (!arch->head && arch->clone.type == art->item->type) {
                    bool allowed = std::none_of(art->allowed.cbegin(), art->allowed.cend(),
                            [&] (const auto name) { return strcmp(arch->name, name + 1) == 0; });
                    if (allowed && arch->clone.face != NULL) {
                        return arch->clone.face->number;
                    }
                }
                arch = get_next_archetype(arch);
            }
            return (uint16_t)-1;
        } else {
            const archetype *arch = try_find_archetype(art->allowed[0]);
            if (arch == NULL) {
                arch = find_archetype_by_object_name(art->allowed[0]);
            }
            if (arch != NULL)
                return arch->clone.face->number;
            return (uint16_t)-1;
        }
    }

    while (arch != NULL) {
        if (arch->clone.type == art->item->type && arch->clone.face != NULL)
            return arch->clone.face->number;

        arch = get_next_archetype(arch);
    }
    return (uint16_t)-1;
}
