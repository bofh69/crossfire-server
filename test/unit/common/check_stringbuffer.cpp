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
 * This is the unit tests file for common/stringbuffer.c
 */

#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "sproto.h"

#include <check.h>

static void setup(void) {
    init_hash_table();
}

static void teardown(void) {
    /* nothing to do */
}

static void check_sb_equals(StringBuffer *sb, const char *value) {
    char *end = stringbuffer_finish(sb);
    ck_assert_str_eq(end, value);
    free(end);
}

START_TEST(test_empty) {
    StringBuffer *sb = stringbuffer_new();
    check_sb_equals(sb, "");
}
END_TEST

START_TEST(test_shared) {
    StringBuffer *sb = stringbuffer_new();
    stringbuffer_append_string(sb, "test");
    sstring end = stringbuffer_finish_shared(sb);
    ck_assert_str_eq(end, "test");
    ck_assert_ptr_ne(find_string("test"), NULL);
    free_string(end);
}
END_TEST

START_TEST(test_append_int64_zero) {
    StringBuffer *sb = stringbuffer_new();
    stringbuffer_append_int64(sb, 0);
    check_sb_equals(sb, "0");
}
END_TEST

START_TEST(test_append_int64_positive) {
    StringBuffer *sb = stringbuffer_new();
    stringbuffer_append_int64(sb, 75387);
    check_sb_equals(sb, "75387");
}
END_TEST

START_TEST(test_append_int64_negative) {
    StringBuffer *sb = stringbuffer_new();
    stringbuffer_append_int64(sb, -778185);
    check_sb_equals(sb, "-778185");
}
END_TEST

START_TEST(test_append_int64_max) {
    StringBuffer *sb = stringbuffer_new();
    stringbuffer_append_int64(sb, INT64_MAX);
    check_sb_equals(sb, "9223372036854775807");
}
END_TEST

START_TEST(test_append_int64_min) {
    StringBuffer *sb = stringbuffer_new();
    stringbuffer_append_int64(sb, INT64_MIN);
    check_sb_equals(sb, "-9223372036854775807");
}
END_TEST

static Suite *stringbuffer_suite(void) {
    Suite *s = suite_create("stringbuffer");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_empty);
    tcase_add_test(tc_core, test_shared);
    tcase_add_test(tc_core, test_append_int64_zero);
    tcase_add_test(tc_core, test_append_int64_positive);
    tcase_add_test(tc_core, test_append_int64_negative);
    tcase_add_test(tc_core, test_append_int64_max);
    tcase_add_test(tc_core, test_append_int64_min);

    return s;
}

int main(void) {
    int nf;
    Suite *s = stringbuffer_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, LOGDIR "/unit/common/stringbuffer.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/stringbuffer.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
