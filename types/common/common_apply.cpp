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
 * This file contains apply-related methods that are common to many
 * classes of objects.
 */

#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sproto.h>

static int ob_move_on_recursion_depth = 0;

method_ret common_pre_ob_move_on(object *trap, object *victim, object *originator) {
    /* If player is DM, only 2 cases to consider:
     * - exits
     * - opened containers on the ground, which should be closed.
     */
    if (QUERY_FLAG(victim, FLAG_WIZPASS)
    && trap->type != EXIT
    && trap->type != SIGN
    && trap->type != CONTAINER
    && !QUERY_FLAG(trap, FLAG_APPLIED))
        return METHOD_ERROR;

    /* The following was changed because it was causing perfectly correct
     * maps to fail.  1)  it's not an error to recurse:
     * rune detonates, summoning monster.  monster lands on nearby rune.
     * nearby rune detonates.  This sort of recursion is expected and
     * proper.  This code was causing needless crashes.
     */
    if (ob_move_on_recursion_depth >= 500) {
        LOG(llevDebug, "WARNING: move_apply(): aborting recursion [trap arch %s, name %s; victim arch %s, name %s]\n", trap->arch->name, trap->name, victim->arch->name, victim->name);
        return METHOD_ERROR;
    }
    ob_move_on_recursion_depth++;
    trap = HEAD(trap);

    if (events_execute_object_event(trap, EVENT_TRIGGER, originator, victim, NULL, SCRIPT_FIX_ALL) != 0) {
        ob_move_on_recursion_depth--;
        return METHOD_ERROR;
    }
    return METHOD_OK;
}

void common_post_ob_move_on(object *trap, object *victim, object *originator) {
    (void)trap;
    (void)victim;
    (void)originator;
    ob_move_on_recursion_depth--;
    if (ob_move_on_recursion_depth < 0) /* Safety net :) */
        ob_move_on_recursion_depth = 0;
}
