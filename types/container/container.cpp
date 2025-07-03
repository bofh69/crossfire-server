/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2007 Mark Wedel & Crossfire Development Team
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/** @file
 * The implementation of the Container class of objects.
 */

#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>

static method_ret container_type_move_on(object *trap, object *victim, object *originator);

bool sack_race_can_contain(const object *sack, const object *ob) {
    if (!sack->race)
        // no restriction
        return true;

    if (!ob->race)
        // sack restricted, but object not defined
        return false;

    return csv_contains(sack->race, ob->race);
}

/**
 * Check if an item op can be put into a sack. If pl exists then tell
 * a player the reason of failure.
 *
 * @param pl
 * player.
 * @param sack
 * container to try to put into.
 * @param op
 * what to put in the sack.
 * @param nrof
 * number of objects (op) we want to put in. We specify it separately instead of
 * using op->nrof because often times, a player may have specified a
 * certain number of objects to drop, so we can pass that number, and
 * not need to use split_ob() and stuff.
 * @return
 * 1 if it will fit, 0 if it will not.
 */
int sack_can_hold(const object *pl, const object *sack, const object *op, uint32_t nrof) {
    char name[MAX_BUF];
    query_name(sack, name, MAX_BUF);

    if (!QUERY_FLAG(sack, FLAG_APPLIED)) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "The %s is not active.",
                             name);
        return 0;
    }
    if (sack == op) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "You can't put the %s into itself.",
                             name);
        return 0;
    }
    if (sack->race
    && (!sack_race_can_contain(sack, op) || op->type == CONTAINER || (sack->stats.food && sack->stats.food != op->type))) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "You can put only %s into the %s.",
                             sack->race,  name);
        return 0;
    }
    if (op->type == SPECIAL_KEY && sack->slaying && op->slaying) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "You can't put the key into %s.",
                             name);
        return 0;
    }
    if (sack->weight_limit) {
        int32_t new_weight;

        new_weight = sack->carrying+(nrof ? nrof : 1)
            /* Most non-containers should have op->carrying == 0. Icecubes, however, will not,
             * and we need to handle those.
             * Neila Hawkins 2021-01-21
             */
            *(op->weight+(op->type == CONTAINER ? op->carrying*op->stats.Str : op->carrying))
            *(100-sack->stats.Str)/100;
        if (new_weight > sack->weight_limit) {
            draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                "That won't fit in the %s!",
                name);
            return 0;
        }
    }
    /* All other checks pass, must be OK */
    return 1;
}

/**
 * Makes an object's face the main face, which is supposed to be the "closed" one.
 *
 * Sets an object's face to the 'face' in the archetype.
 * Meant for showing containers opening and closing.
 *
 * @param op
 * Object to set face on
 *
 * @return TRUE if face changed
 */
int set_object_face_main(object *op) {
    const Face *newface = op->arch->clone.face;
    sstring saved = object_get_value(op, "face_closed");

    if (op->more)
        set_object_face_main(op->more);

    if (saved)
        newface = try_find_face(saved, newface);
    if (newface && op->face != newface) {
        op->face = newface;
        return TRUE;
    }
    return FALSE;
}

/**
 * Makes an object's face the other_arch face, supposed to be the "opened" one.
 *
 * Sets an object's face to the other_arch 'face'.
 * Meant for showing containers opening and closing.
 *
 * @param op
 * Object to set face on
 *
 * @return TRUE if face changed
 */
static int set_object_face_other(object *op) {
    sstring custom;
    const Face *newface = NULL;
    object *head = op->head ? op->head : op;

    if (op->more)
        set_object_face_other(op->more);

    if (head->face && head->other_arch && head->other_arch->clone.face)
        newface = head->other_arch->clone.face;

    if (op->face != op->arch->clone.face) {
        /* object has a custom face, save it so it gets correctly restored later. */
        object_set_value(op, "face_closed", op->face->name, 1);
    }

    custom = object_get_value(head, "face_opened");
    if (custom)
        newface = try_find_face(custom, newface);
    if (newface && op->face != newface) {
        op->face = newface;
        return TRUE;
    }
    return FALSE;
}

/**
 * Handle apply on containers.  This is for
 * containers that are applied by a player, whether in inventory or
 * on the ground: eg, sacks, luggage, etc.
 *
 * Moved to own function and added many features [Tero.Haatanen(at)lut.fi]
 * This version is for client/server mode.
 *
 * Reminder - there are three states for any container - closed (non applied),
 * applied (not open, but objects that match get tossed into it), and open
 * (applied flag set, and op->container points to the open container)
 *
 * @param op
 * player.
 * @param sack
 * container the player is opening or closing.
 * @param aflags
 * combination of @ref AP_xxx flags.
 * @return
 * 1 if an object is applied somehow or another, 0 if error/no apply
 *
 * @author Eneq(at)(csd.uu.se)
 */
int apply_container(object *op, object *sack, int aflags) {
    char name_sack[MAX_BUF], name_tmp[MAX_BUF];
    object *tmp = op->container;

    if (op->type != PLAYER)
        return 0; /* This might change */

    if (sack == NULL || sack->type != CONTAINER) {
        LOG(llevError, "apply_container: '%s' tried to apply %s, which is not a container\n", op->name, sack ? sack->name : "(null)");
        return 0;
    }

    if (sack->head)
        sack = sack->head;

    query_name(sack, name_sack, MAX_BUF);

    if ( aflags == AP_APPLY || aflags == AP_UNAPPLY || aflags == AP_OPEN ) {
        // What if the container isn't in the player's inventory?
        if ( sack->env != op ) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                                 "Not in your inventory: %s",
                                 name_sack);
            return 0;
        }
    }
    if ( aflags == AP_APPLY ) {
        // What if the container is open?  Make it just ready!
        if ( op->container == sack ) {
            op->container = NULL;
            if (op->contr != NULL)
                op->contr->socket->container_position = 0;
            CLEAR_FLAG(sack, FLAG_APPLIED);
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                                 "You readied %s.",
                                 name_sack);
            SET_FLAG(sack, FLAG_APPLIED);
            // FIXME: This is not flipping the face!
            if (set_object_face_main(sack)) // change image to closed
                esrv_update_item(UPD_FLAGS|UPD_FACE, op, sack);
            else
                esrv_update_item(UPD_FLAGS, op, sack);
            return 0;
        }
        // What if the container is already applied?  Do nothing!
        if (QUERY_FLAG(sack, FLAG_APPLIED)) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                                 "Already readied %s.",
                                 name_sack);
            return 0;
        }
        // What if the container is closed?  Same as no aflags.
        aflags = AP_NULL;
    }

    if ( aflags == AP_OPEN ) {
        // What if the container is already open?
        if ( op->container == sack ) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                                 "Already opened %s.",
                                 name_sack);
            return 0;
        }
        // Set the container as applied and then proceed as if no special flags
        SET_FLAG(sack, FLAG_APPLIED);
        aflags = AP_NULL;
    }

    if ( aflags == AP_UNAPPLY ) {
        // If not open, two cases:
        if ( op->container != sack ) {
            if (QUERY_FLAG(sack, FLAG_APPLIED)) {
                CLEAR_FLAG(sack, FLAG_APPLIED);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                                     "You closed %s.",
                                     name_sack);
                esrv_update_item(UPD_FLAGS, op, sack);
                return 0;
            }
            else {
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                                     "Already closed %s.",
                                     name_sack);
                return 0;
            }
        }
        // open; same as no special flags
        aflags = AP_NULL;
    }

    /* If we have a currently open container, then it needs
     * to be closed in all cases if we are opening this one up.
     * We then fall through if appropriate for opening the new
     * container.
     */
    if (op->container && QUERY_FLAG(op->container, FLAG_APPLIED) &&
        (QUERY_FLAG(sack, FLAG_APPLIED) || sack->env != op) )
    {
        tag_t tmp_tag = op->container->count;

        if (op->container->env != op) { /* if container is on the ground */
            object *part = op->container->head ? op->container->head : op->container;
            while (part) {
                part->move_off = 0;
                part = part->more;
            }
        }

        /* Query name before the close event, as the container could be destroyed. */
        query_name(op->container, name_tmp, MAX_BUF);

        if (events_execute_object_event(tmp, EVENT_CLOSE, op, NULL, NULL, SCRIPT_FIX_ALL) != 0)
            return 1;

        draw_ext_info_format(NDI_UNIQUE, 0, op,
                             MSG_TYPE_APPLY, MSG_TYPE_APPLY_UNAPPLY,
                             "You close %s.",
                             name_tmp);

        op->container = NULL;
        if (op->contr != NULL)
            op->contr->socket->container_position = 0;

        /* The container may have been destroyed by the event handler. */
        if (!object_was_destroyed(tmp, tmp_tag)) {
            CLEAR_FLAG(tmp, FLAG_APPLIED);
            if (set_object_face_main(tmp))
                esrv_update_item(UPD_FLAGS|UPD_FACE, op, tmp);
            else
                esrv_update_item(UPD_FLAGS, op, tmp);
        }
        if (tmp == sack)
            return 1;
    }

    /* If the player is trying to open it (which he must be doing
     * if we got here), and it is locked, check to see if player
     * has the equipment to open it.
     */

    if (sack->slaying) { /* it's locked */
        tmp = find_key(op, op, sack);
        if (tmp) {
            query_name(tmp, name_tmp, MAX_BUF);
            draw_ext_info_format(NDI_UNIQUE, 0, op,
                                 MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                                 "You unlock %s with %s.",
                                 name_sack, name_tmp);
        } else {
            draw_ext_info_format(NDI_UNIQUE, 0, op,
                                 MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                                 "You don't have the key to unlock %s.",
                                 name_sack);
            return 0;
        }
    }

    /* By the time we get here, we have made sure any other container
     * has been closed and if this is a locked container, the player
     * has the key to open it.
     */

    /* There are really two cases - the sack is either on the ground,
     * or the sack is part of the player's inventory.  If on the ground,
     * we assume that the player is opening it, since if it was being
     * closed, that would have been taken care of above.
     */


    if (sack->env != op) {
        /* Hypothetical case - the player is trying to open a sack
         * that belongs to someone else.  This normally should not
         * happen, but a misbehaving client/player could
         * try to do it, so let's handle it gracefully.
         */
        if (sack->env) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                                 "You can't open %s",
                                 name_sack);
            return 0;
        }

        if (sack->nrof > 1) {
            object *left = object_split(sack, sack->nrof-1, NULL, 0);

            object_insert_in_map_at(left, sack->map, NULL, INS_NO_MERGE, sack->x, sack->y);
            /* recompute the name so it's nice */
            query_name(sack, name_sack, MAX_BUF);
        }

        /* set it so when the player walks off, we can unapply the sack */
        {
            object *part = sack->head ? sack->head : sack;
            while (part) {
                part->move_off = MOVE_ALL;
                part = part->more;
            }
        }

        CLEAR_FLAG(sack, FLAG_APPLIED);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                             "You open %s.",
                             name_sack);
        SET_FLAG(sack, FLAG_APPLIED);
        op->container = sack;
        if (op->contr != NULL)
            op->contr->socket->container_position = 0;

        if (set_object_face_other(sack))
            esrv_update_item(UPD_FLAGS|UPD_FACE, op, sack);
        else
            esrv_update_item(UPD_FLAGS, op, sack);
        esrv_send_inventory(op, sack);
    } else { /* sack is in players inventory */
        if (QUERY_FLAG(sack, FLAG_APPLIED)) {  /* readied sack becoming open */
            CLEAR_FLAG(sack, FLAG_APPLIED);
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                                 "You open %s.",
                                 name_sack);
            SET_FLAG(sack, FLAG_APPLIED);
            op->container = sack;
            if (op->contr != NULL)
                op->contr->socket->container_position = 0;

            if (set_object_face_other(sack))
                esrv_update_item(UPD_FLAGS|UPD_FACE, op, sack);
            else
                esrv_update_item(UPD_FLAGS, op, sack);
            esrv_send_inventory(op, sack);
        } else {
            object *left = NULL;

            if (sack->nrof > 1)
                left = object_split(sack, sack->nrof-1, NULL, 1);

            CLEAR_FLAG(sack, FLAG_APPLIED);
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                                 "You readied %s.",
                                 name_sack);
            SET_FLAG(sack, FLAG_APPLIED);
            esrv_update_item(UPD_FLAGS, op, sack);

            if (left) {
                object_insert_in_ob(left, sack->env);
                esrv_send_item(op, left);
            }
        }
    }
    return 1;
}

method_ret container_apply(object *op, object *applier, int aflags) {
    if (applier->type == PLAYER)
        (void)apply_container(applier, op, aflags);
    return METHOD_OK;
}

method_ret close_container_apply(object *op, object *applier, int aflags) {
    if (applier->type == PLAYER)
        (void)apply_container(applier, op->env, aflags);
    return METHOD_OK;
}

void container_describe(const object *op, const object *observer, int use_media_tags, char *buf, size_t size) {
    legacy_ob_describe(op, observer, use_media_tags, buf, size);
    if (((op->env && op->env->container == op) || (!op->env && QUERY_FLAG(op, FLAG_APPLIED))))
        strncat(buf, " (open)", size);
}

method_ret container_examine(const object *op, const object *observer, int use_media_tags, char *buf, size_t size) {
    if (op->race != NULL) {
        if (op->weight_limit && op->stats.Str < 100)
            snprintf(buf, size, "It can hold only %s and its weight limit is %.1f kg.", op->race, op->weight_limit/(10.0*(100-op->stats.Str)));
        else
            snprintf(buf, size, "It can hold only %s.", op->race);
    } else
        if (op->weight_limit && op->stats.Str < 100)
            snprintf(buf, size, "Its weight limit is %.1f kg.", op->weight_limit/(10.0*(100-op->stats.Str)));
    return METHOD_OK;
}

/**
 * Initializer for the CONTAINER object type.
 */
void init_type_container(void) {
    register_move_on(CONTAINER, container_type_move_on);
    register_apply(CONTAINER, container_apply);
    register_describe(CONTAINER, container_describe);
    register_examine(CONTAINER, container_examine);
    register_apply(CLOSE_CON, close_container_apply);
}
/**
 * Move on this Container object.
 * @param trap The Container we're moving on
 * @param victim The object moving over this one
 * @param originator The object that caused the move_on event
 * @return METHOD_OK
 */
static method_ret container_type_move_on(object *trap, object *victim, object *originator) {
    if (common_pre_ob_move_on(trap, victim, originator) == METHOD_ERROR)
        return METHOD_OK;
    if (victim->type == PLAYER)
        (void)apply_container(victim, trap, AP_NULL);
    common_post_ob_move_on(trap, victim, originator);
    return METHOD_OK;
}
