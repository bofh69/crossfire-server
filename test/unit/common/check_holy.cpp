/*
 * CrossFire, A Multiplayer game for X-windows
 *
 * Copyright (C) 2022 the Crossfire Development Team
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
 * This is the unit tests file for common/holy.c
 */

#include <stdlib.h>
#include <global.h>
#include <assert.h>
#include <toolkit_common.h>

#include <check.h>

void setup(void) {
    cctk_setdatadir(SOURCE_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/common/holy.out");
    cctk_init_std_archetypes();
    init_gods();
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST(test_get_random_god) {
    const object *ob = get_rand_god();
    FAIL_UNLESS(ob != NULL, "There must be at least one god!");
}
END_TEST

START_TEST(test_find_god) {
    const object *ob = find_god("Valriel");
    FAIL_UNLESS(ob != NULL, "Valriel must exist");
}
END_TEST

START_TEST(test_find_god_invalid) {
    const object *ob = find_god("FlyingSpaghettiMonster");
    FAIL_UNLESS(ob == NULL, "FSM must not be found");
}
END_TEST

START_TEST(test_get_god_for_race) {
    const char *god = get_god_for_race("undead");
    FAIL_UNLESS(god != NULL, "Undead must have a race");
    ck_assert_str_eq(god, "Devourers");
}
END_TEST

START_TEST(test_get_god_for_race_invalid) {
    const char *god = get_god_for_race("unicorns!!");
    FAIL_UNLESS(god == NULL, "Must not find a god");
}
END_TEST

Suite *holy_suite(void) {
    Suite *s = suite_create("holy");
    TCase *tc_core = tcase_create("Core");
    tcase_set_timeout(tc_core, 60);

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_get_random_god);
    tcase_add_test(tc_core, test_find_god);
    tcase_add_test(tc_core, test_find_god_invalid);
    tcase_add_test(tc_core, test_get_god_for_race);
    tcase_add_test(tc_core, test_get_god_for_race_invalid);

    return s;
}

int main(void) {
    int nf;
    Suite *s = holy_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, LOGDIR "/unit/common/holy.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/holy.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
