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
 * The implementation of the Converter class of objects.
 */

#include "global.h"

#include <string.h>
#include <stdlib.h>

#include "ob_methods.h"
#include "ob_types.h"
#include "shop.h"
#include "sounds.h"
#include "sproto.h"

/*
 * convert_item() returns 1 if anything was converted, 0 if the item was not
 * what the converter wants, -1 if the converter is broken.
 */
#define CONV_FROM(xyz)  xyz->slaying
#define CONV_TO(xyz)    xyz->other_arch
#define CONV_NR(xyz)    (unsigned char)xyz->stats.sp
#define CONV_LIMIT(xyz) ((xyz->stats.wc > 0) ? ((unsigned long)xyz->stats.wc) : INT32_MAX)
#define CONV_NEED(xyz)  (unsigned long)xyz->stats.food
#define CONV_FROM_MATCH(xyz,_match) (CONV_FROM(xyz) == (_match) || ((strchr(CONV_FROM(xyz),'*')) && wildcard_match(CONV_FROM(xyz),(_match))))

static int convert_item(object *item, object *converter);

/**
 * Wildcard match where 'string' contains a '*' at the start, end, or both
 */
static int wildcard_match(const char *string,const char *candidate)
{
   bool head=FALSE,tail=FALSE;
   char *str;
   char *m;
   bool ret;

   if ( string[0]=='*' && string[1]==0 ) return 1; // Trivial match-all case
   if ( string[0] == '*' ) head=TRUE;
   if ( string[strlen(string)-1] == '*' ) tail=TRUE;
   str=strdup(&string[head?1:0]);
   if ( !str ) return FALSE;
   if ( tail ) str[strlen(str)-1]=0;
   /* 'str' is now the text to match without the wildcard */
   ret=FALSE;
   if ( head && tail ) {
      m=strstr(str,candidate);
      if ( m ) ret=TRUE;
   } else if ( tail ) {
      if ( strncmp(str,candidate,strlen(str)) == 0 ) ret=TRUE;
   } else {
      if ( strlen(candidate) >= strlen(str) &&
           strcmp(&candidate[strlen(candidate)-strlen(str)],str) == 0 ) ret=TRUE;
   }
   free(str);
   return ret;
}

static method_ret converter_type_move_on(object *trap, object *victim, object *originator);
static method_ret converter_type_apply(object *converter, object *applier, int aflags);

/**
 * Initializer for the CONVERTER object type.
 */
void init_type_converter(void) {
    register_move_on(CONVERTER, converter_type_move_on);
    register_apply(CONVERTER, converter_type_apply);
}

/**
 * Do the actual item conversion.
 * Determine the number of sources item to remove and the number of result items to produce.
 *
 * @param converter The converter being used
 * @param item The discovered source item object.
 * @param weight_rem The amount of weight we can add to the total. Used when doing direct-in-player-inventory conversions. Can be set to -1 if there is no weight limit, such as when using move_on.
 * @param status The conversion status. 1 if successful, 0 if no conversion nor error, -1 if error. Caller cannot pass NULL here.
 *
 * @return A pointer to the produced object, which needs to be appropriately insert into map or inventory. Returns NULL if an error or no conversion happened.
 */
static object *do_item_conversion(object *converter, object *item, int weight_rem, int *status) {
    int nr = 0;
    uint32_t price_in;
    // item->weight holds the weight of the whole stack.
    // Negative weight_diff means the output weighs less than the input, so it can be done indefinitely. Positive weight means that, if done from in-inventory, only enough conversions to make up the weight difference can be allowed.
    // TODO: Handle in containers and the effective weight reduction that occurs there.
    int32_t weight_diff = (CONV_NR(converter) ? CONV_NR(converter) : 1)*converter->other_arch->clone.weight - (CONV_NEED(converter) ? CONV_NEED(converter) : 1)*item->weight;
    // -1 means we have no weight-imposed limit.
    int32_t max_conv = weight_diff > 0 && weight_rem > 0 ? weight_rem / weight_diff : -1;
    if (max_conv == 0) {
        // TODO: Send a message to the player saying they cannot carry the converted item.
        *status = 0;
        return NULL;
    }

    /* We make some assumptions - we assume if it takes money as it type,
     * it wants some amount.  We don't make change (ie, if something costs
     * 3 gp and player drops a platinum, tough luck)
     *
     * We can also assert that changers that take "money" will not perform on apply events, due to added complexity.
     */
    if (!strcmp(CONV_FROM(converter), "money")) {
        int cost;

        if (item->type != MONEY) {
            *status = 0;
            return NULL;
        }

        nr = MIN(CONV_LIMIT(converter), (item->nrof*item->value)/CONV_NEED(converter));
        if (!nr) {
            *status = 0;
            return NULL;
        }

        cost = nr*CONV_NEED(converter)/item->value;
        /* take into account rounding errors */
        if (nr*CONV_NEED(converter)%item->value)
            cost++;
        object_decrease_nrof(item, cost);

        price_in = cost*item->value;
    } else {
        if (item->type == PLAYER
        || !CONV_FROM_MATCH(converter,item->arch->name)
        || (CONV_NEED(converter) && CONV_NEED(converter) > item->nrof)) {
            *status = 0;
            return NULL;
        }

        /* silently burn unpaid items (only if they match what we want) */
        if (QUERY_FLAG(item, FLAG_UNPAID)) {
            object_remove(item);
            object_free_drop_inventory(item);
            item = create_archetype("burnout");
            *status = 1;
            return item;
        }

        if (CONV_NEED(converter)) {
            nr = MIN(CONV_LIMIT(converter), item->nrof/CONV_NEED(converter));
            // If there is an inventory weight restriction, that supercedes the amount that could be created.
            if (max_conv != -1)
                nr = MIN(nr, max_conv);
            object_decrease_nrof(item, nr*CONV_NEED(converter));
            price_in = nr*CONV_NEED(converter)*item->value;
        } else {
            price_in = item->value;
            object_remove(item);
            object_free_drop_inventory(item);
        }
    }

    if (converter->inv != NULL) {
        int i;
        object *ob_to_copy;

        /* select random object from inventory to copy */
        ob_to_copy = converter->inv;
        i = 1;
        FOR_BELOW_PREPARE(converter->inv, ob) {
            if (rndm(0, i) == 0)
                ob_to_copy = ob;
            i++;
        } FOR_BELOW_FINISH();
        item = object_create_clone(ob_to_copy);
        CLEAR_FLAG(item, FLAG_IS_A_TEMPLATE);
        object_unset_flag_inv(item, FLAG_IS_A_TEMPLATE);
    } else {
        if (converter->other_arch == NULL) {
            LOG(llevError, "move_creator: Converter doesn't have other arch set: %s (%s, %d, %d)\n", converter->name ? converter->name : "(null)", converter->map->path, converter->x, converter->y);
            *status = -1;
            return NULL;
        }
        item = object_create_arch(converter->other_arch);
        fix_generated_item(item, converter, 0, 0, GT_MINIMAL);
    }

    if (CONV_NR(converter))
        item->nrof = CONV_NR(converter);
    if (nr)
        item->nrof *= nr;
    if (item->type != MONEY && shop_contains(converter))
        SET_FLAG(item, FLAG_UNPAID);
    else if (price_in < item->nrof*item->value && settings.allow_broken_converters == FALSE) {
        LOG(llevError, "Broken converter %s at %s (%d, %d) in value %d, out value %d for %s\n", converter->name, converter->map->path, converter->x, converter->y, price_in, item->nrof*item->value, item->name);
        object_free_drop_inventory(item);
        *status = -1;
        return NULL;
    }
    // If we get here, the conversion was successful.
    *status = 1;
    return item;
}

/**
 * Transforms an item into another item.
 * @param item The object that triggered the converter - if it isn't of a type
 * accepted by the converter, nothing will happen
 * @param converter The object that is doing the conversion
 * @retval -1 If something went wrong when attempting the conversion
 * @retval 0 If the item was not converted
 * @retval 1 If the item got converted
 */
static int convert_item(object *item, object *converter) {
    int status; // Return for the result status of good, bad, or otherwise.
    item = do_item_conversion(converter, item, -1, &status);
    if (status == 1)
        object_insert_in_map_at(item, converter->map, converter, 0, converter->x, converter->y);
    return status;
}

/**
 * Move on this Converter object.
 * @param trap The Converter we're moving on
 * @param victim The object moving over this one
 * @param originator The object that caused the move_on event
 * @return METHOD_OK
 */
static method_ret converter_type_move_on(object *trap, object *victim, object *originator) {
    if (common_pre_ob_move_on(trap, victim, originator) == METHOD_ERROR)
        return METHOD_OK;
    if (convert_item(victim, trap) < 0) {
        object *op;
        char name[MAX_BUF];

        query_name(trap, name, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, originator, MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
            "The %s seems to be broken!", name);

        op = create_archetype("burnout");
        if (op != NULL)
            object_insert_in_map_at(op, trap->map, trap, 0, trap->x, trap->y);
    }
    common_post_ob_move_on(trap, victim, originator);
    return METHOD_OK;
}

/**
 * Apply the converter object.
 *
 * Looks for the desired type of the converter in top-level inventory and active containers,
 * and performs the conversion until inventory is full or until conversion is exhausted.
 *
 * @param converter The converter we applied
 * @param applier The player applying the converter.
 * @param aflags The apply flags (currently unused)
 * @return METHOD_OK
 */
static method_ret converter_type_apply(object *converter, object *applier, int aflags) {
    (void)aflags; // Unused
    object *conv_src = object_find_by_arch_name(applier, CONV_FROM(converter));
    if (conv_src != NULL) {
        int status;
        // Handle player weight limits
        int lim = applier->type == PLAYER ? get_weight_limit(applier->stats.Str) - applier->carrying : -1;
        object *conv_res = do_item_conversion(converter, conv_src, lim, &status);
        if (conv_res != NULL)
            object_insert_in_ob(conv_res, applier);
    }
    return METHOD_OK;
}
