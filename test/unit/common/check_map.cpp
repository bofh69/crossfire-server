/*
 * static char *rcsid_check_map_c =
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
 * This is the unit tests file for common/map.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "global.h"

#include <check.h>
#include "toolkit_common.h"

void setup(void) {
    cctk_setdatadir(SOURCE_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/common/map.out");
    init_experience(); // need level table to load some maps
    cctk_init_std_archetypes();
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

/**
 * Compare two files (by path) for equality.
 */
int cmp(std::string src, std::string dst) {
    // std::format() is only available in C++20...
    std::string cmd = "cmp";
    cmd += " ";
    cmd += src;
    cmd += " ";
    cmd += dst;
    return system(cmd.c_str());
}

/**
 * Load a map, then save it and compare the two files.
 */
void test_load_save_regular_map(std::string map) {
    std::string dst = "check_map.out";
    struct mapstruct *m = ready_map_name(map.c_str(), MAP_FLUSH); // load from scratch
    FILE *fp;
    fp = fopen(dst.c_str(), "w");
    ck_assert_ptr_nonnull(fp);
    int res = save_map_to_stream(m, 0, fp, fp);
    fclose(fp);
    ck_assert_int_ge(res, 0);
    std::string src = SOURCE_ROOT "lib/maps" + map;
    ck_assert_int_eq(cmp(src, dst), 0);
}

START_TEST(test_map_save) {
    test_load_save_regular_map("/world/world_104_115");
    test_load_save_regular_map("/world/world_104_116");
    test_load_save_regular_map("/world/world_105_115");
    //test_load_save_regular_map("/world/world_105_116");
}
END_TEST

Suite *map_suite(void) {
    Suite *s = suite_create("map");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_map_save);

    return s;
}

int main(void) {
    int nf;
    Suite *s = map_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, LOGDIR "/unit/common/map.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/map.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
