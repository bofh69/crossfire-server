/*
 * static char *rcsid_check_recipe_c =
 *   "$Id$";
 */

/*
 * CrossFire, A Multiplayer game for X-windows
 *
 * Copyright (C) 2002 Mark Wedel & Crossfire Development Team
 * Copyright (C) 1992 Frank Tore Johansen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * The authors can be reached via e-mail at crossfire-devel@real-time.com
 */

/*
 * This is the unit tests file for common/recipe.c
 */

#include <stdlib.h>

#include "global.h"
#include <toolkit_common.h>

#include <check.h>

void setup(void) {
    cctk_setdatadir(SOURCE_ROOT"lib");
    cctk_setlog(LOGDIR"/unit/common/arch.out");
    cctk_init_std_archetypes();
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST(test_check_recipes) {
    FAIL_UNLESS(check_recipes(), "check_recipes failed");
}
END_TEST

START_TEST(test_check_formulae) {
    FAIL_UNLESS(check_formulae(), "check_formulae failed");
}
END_TEST

Suite *recipe_suite(void) {
    Suite *s = suite_create("recipe");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_check_recipes);
    tcase_add_test(tc_core, test_check_formulae);

    return s;
}

int main(void) {
    int nf;
    Suite *s = recipe_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, LOGDIR "/unit/common/recipe.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/recipe.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
