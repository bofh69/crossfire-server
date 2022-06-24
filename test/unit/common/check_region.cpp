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
 * This is the unit tests file for common/region.c
 */

#include <stdlib.h>
#include <check.h>
#include <global.h>
#include <assert.h>
#include <toolkit_common.h>

void setup(void) {
    cctk_setdatadir(SOURCE_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/common/region.out");
    cctk_init_std_archetypes();
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST(test_get_by_unknown_name) {
    region *reg = get_region_by_name("undefined?!");
    fail_unless(reg != NULL, "Must get a region for all names");
    fail_unless(reg->fallback, "Region must be fallback");
}
END_TEST

START_TEST(test_get_by_name) {
    region *reg = get_region_by_name("scorn");
    fail_unless(reg != NULL, "Must get a region for scorn");
    fail_unless(!reg->fallback, "Region must not be fallback");
}
END_TEST

START_TEST(test_get_region_from_string_empty) {
    region *reg = get_region_from_string("");
    fail_unless(reg != NULL, "Must get a region for empty");
    fail_unless(!reg->parent, "Region must not have a parent");
}
END_TEST

START_TEST(test_get_region_from_string_exact_name) {
    region *reg = get_region_from_string("darcapcircus");
    fail_unless(reg != NULL, "Must get a region for darcapcircus");
    fail_unless(!reg->fallback, "Region must not be fallback");
}
END_TEST

START_TEST(test_get_region_from_string_exact_name_wrong_case) {
    region *reg = get_region_from_string("darcapciRcus");
    fail_unless(reg != NULL, "Must not get a region for darcapciRcus");
    fail_unless(!reg->fallback, "Region must not be fallback");
}
END_TEST

START_TEST(test_get_region_from_string_partial_name) {
    region *reg = get_region_from_string("uthv");
    fail_unless(reg != NULL, "Must get a region for uthv");
    fail_unless(!reg->fallback, "Region must not be fallback");
}
END_TEST

START_TEST(test_get_region_from_string_exact_long_name) {
    region *reg = get_region_from_string("Team Arena Volcano");
    fail_unless(reg != NULL, "Must get a region for Team Arena Volcano");
    fail_unless(!reg->fallback, "Region must not be fallback");
}
END_TEST

START_TEST(test_get_region_from_string_partial_long_name) {
    region *reg = get_region_from_string("lord marksel");
    fail_unless(reg != NULL, "Must get a region for lord marksel");
    fail_unless(!reg->fallback, "Region must not be fallback");
}
END_TEST

START_TEST(test_check_region_parent) {
    region *reg = get_region_from_string("scorncounty");
    fail_unless(reg != NULL, "Must get a region for scorncounty");
    fail_unless(!reg->fallback, "Region must not be fallback");
    fail_unless(reg->parent != NULL, "scorncounty must have a parent");
    fail_unless(strcmp(reg->parent->name, "scorn") == 0, "scorncounty must be child of scorn");
    fail_unless(reg->parent->parent != NULL, "Scorn must be a child too");
    fail_unless(strcmp(reg->parent->parent->name, "world") == 0, "scorn must be child of world");
}
END_TEST

START_TEST(test_get_region_is_child_of_region) {
    region *reg1 = get_region_from_string("scorn");
    region *reg2 = get_region_from_string("scorncounty");
    fail_unless(region_is_child_of_region(reg2, reg1), "scorncountry should be child of scorn");
}
END_TEST

Suite *region_suite(void) {
    Suite *s = suite_create("region");
    TCase *tc_core = tcase_create("Core");
    tcase_set_timeout(tc_core, 60);

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_get_by_unknown_name);
    tcase_add_test(tc_core, test_get_by_name);
    tcase_add_test(tc_core, test_get_region_from_string_empty);
    tcase_add_test(tc_core, test_get_region_from_string_exact_name);
    tcase_add_test(tc_core, test_get_region_from_string_exact_name_wrong_case);
    tcase_add_test(tc_core, test_get_region_from_string_partial_name);
    tcase_add_test(tc_core, test_get_region_from_string_exact_long_name);
    tcase_add_test(tc_core, test_get_region_from_string_partial_long_name);
    tcase_add_test(tc_core, test_check_region_parent);
    tcase_add_test(tc_core, test_get_region_is_child_of_region);

    return s;
}

int main(void) {
    int nf;
    Suite *s = region_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, LOGDIR "/unit/common/region.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/region.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    srunner_set_fork_status(sr, CK_NOFORK);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
