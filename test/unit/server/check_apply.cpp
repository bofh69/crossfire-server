/*
 * static char *rcsid_check_apply_c =
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
 * This is the unit tests file for server/apply.c
 */

#include "global.h"

#include <stdlib.h>
#include <check.h>

#include "toolkit_common.h"
#include "sproto.h"

void setup(void) {
    cctk_setdatadir(SOURCE_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/server/check_apply.out");
    cctk_init_std_archetypes();
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST(test_csv_contains) {
    ck_assert(csv_contains("Alice,Bob,Carol", "Alice"));
    ck_assert(csv_contains("Alice,Bob,Carol", "Bob"));
    ck_assert(csv_contains("Alice,Bob,Carol", "Carol"));
    ck_assert(!csv_contains("Alice,Bob,Carol", "Mallory"));
    ck_assert(csv_contains("Apple Pie", "Apple Pie"));
}
END_TEST

START_TEST(test_pouch_holds_stuff) {
    object *pouch = create_archetype("pouch");
    object *money = create_archetype("silvercoin");
    object *gem = create_archetype("gem");
    object *junk = create_archetype("slag");
    SET_FLAG(pouch, FLAG_APPLIED);
    ck_assert(sack_can_hold(NULL, pouch, money, 1));
    ck_assert(sack_can_hold(NULL, pouch, gem, 1));
    ck_assert(!sack_can_hold(NULL, pouch, junk, 1));
}
END_TEST

Suite *apply_suite(void) {
    Suite *s = suite_create("apply");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_csv_contains);
    tcase_add_test(tc_core, test_pouch_holds_stuff);

    return s;
}

int main(void) {
    int nf;
    Suite *s = apply_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, LOGDIR "/unit/server/apply.xml");
    srunner_set_log(sr, LOGDIR "/unit/server/apply.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
