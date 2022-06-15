/*
 * static char *rcsid_check_treasure_c =
 *   "$Id$";
 */

/*
 * CrossFire, A Multiplayer game for X-windows
 *
 * Copyright (C) 2002,2011 Mark Wedel & Crossfire Development Team
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
 * This is the unit tests file for common/treasure.c
 */

#include <stdlib.h>
#include <check.h>
#include <global.h>
#include <toolkit_common.h>

static void setup(void) {
    cctk_setdatadir(SOURCE_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/common/object.out");
    printf("set log to %s\n", LOGDIR"/unit/common/object.out");
    cctk_init_std_archetypes();
}

static void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST(test_add_treasure_in_list) {
    treasurelist list;
    list.items = NULL;
    treasure *first = treasure_insert(&list, 10);
    fail_unless(list.items == first, "First not inserted in first place");

    treasure *new_first = treasure_insert(&list, -1);
    fail_unless(list.items == new_first, "First not replaced");
    fail_unless(new_first->next == first, "List not correctly linked");

    treasure *second = treasure_insert(&list, 11);
    fail_unless(first->next == second, "Second not inserted at end");
    fail_unless(second->next == NULL, "Second should be the last");

    treasure *between = treasure_insert(&list, 2);
    fail_unless(first->next == between, "Between should be after first");
    fail_unless(between->next == second, "Between should be before second");

    treasure *other_first = treasure_insert(&list, 0);
    fail_unless(list.items == other_first, "Other first should be first item");
    fail_unless(other_first->next == new_first, "Other first should be before new first");
}
END_TEST

START_TEST(test_treasure_remove_item) {
    treasurelist list;
    memset(&list, 0, sizeof(list));
    /*treasure *first =*/ treasure_insert(&list, 0);
    treasure *second = treasure_insert(&list, 1);
    /*treasure *third =*/ treasure_insert(&list, 2);
    treasure *fourth = treasure_insert(&list, 3);
    /*treasure *fifth =*/ treasure_insert(&list, 4);

    treasure_remove_item(&list, 0);
    fail_unless(list.items == second, "first not removed");
    treasure_remove_item(&list, 1);
    fail_unless(second->next == fourth, "third not removed");
    treasure_remove_item(&list, 2);
    fail_unless(fourth->next == NULL, "fifth not removed");
}
END_TEST

static void check_treasure_inv(object *op, const char **items) {
    object *inv = op->inv;
    while ((*items) != NULL) {
        fail_unless(inv != NULL, "missing item %s", *items);
        fail_unless(strcmp(inv->name, *items) == 0, "got %s instead of %s", inv->name, *items);
        ++items;
        inv = inv->below;
    }
    if (inv) {
        while (inv) {
            printf(" => unexpected inv %s\n", inv->name);
            inv = inv->below;
        }
        fail_unless(inv != NULL, "got extra inv");
    }
}

const char *empty[] = {NULL};

START_TEST(test_create_treasure_one) {
    const char *items[] = {"kobold's heart", NULL};
    cf_srandom(145);
    treasurelist *list = find_treasurelist("ape_parts");
    fail_unless(list, "missing list 'ape_parts'");
    object *k = create_archetype("kobold");
    fail_unless(k, "missing kobold");
    check_treasure_inv(k, empty);
    create_treasure(list, k, 0, 0, 0);
    check_treasure_inv(k, items);
}
END_TEST

START_TEST(test_create_treasure_all) {
    const const char *items[] = {
        "arrow",
        "bow",
        "ring",
        "plate mail",
        "long sword",
        "kobold's finger",
        "use magic item",
        NULL,
    };
    cf_srandom(94);
    treasurelist *list = find_treasurelist("c_knight");
    fail_unless(list, "missing list 'c_knight'");
    object *k = create_archetype("kobold");
    fail_unless(k, "missing kobold");
    check_treasure_inv(k, empty);
    create_treasure(list, k, 0, 0, 0);
    check_treasure_inv(k, items);
}
END_TEST

static bool check_treasure_arch(object *op, const char **first, const char **last) {
    object *inv = op->inv;
    fail_unless(inv, "missing inv");
    fail_unless(inv->below == NULL, "unexpected below");
    while (first != last) {
        if (strcmp(*first, inv->arch->name) == 0) {
            return true;
        }
        ++first;
    }
    return false;
}

START_TEST(test_magic_limit) {
    const const char *possible[] = {
    // magical traps
    "rune_medium_fireball",
    "rune_burning_hands",
    "rune_poison_cloud",
    "rune_create_bomb",
    "rune_paralysis",
    "rune_shock",
    "rune_confusion",
    "rune_icestorm",
    "rune_drain_magic",
    "rune_blast",
    "rune_fire",
    "rune_frost",
    // nastier traps
    "rune_summon_air_elemental",
    "rune_dragonbreath",
    "rune_large_icestorm",
    "rune_large_fireball",
    "rune_ball_lightning",
    "rune_summon_devil",
    "rune_summon_earth_elemental",
    "rune_summon_fire_elemental",
    "rune_summon_water_elemental",
    "rune_death",
    NULL};

    cf_srandom(19);
    treasurelist *list = find_treasurelist("magical_traps");
    fail_unless(list, "missing list");
    for (int i = 0; i < 1000; i++) {
        object *k = create_archetype("kobold");
        create_treasure(list, k, GT_INVISIBLE, 0, 0);
        if (!k->inv) {
            continue;
        }
        fail_unless(check_treasure_arch(k, possible, possible + 12), "wrong inv %s", k->inv->arch->name);
    }

    int nastier = 0;
    for (int i = 0; i < 1000; i++) {
        object *k = create_archetype("kobold");
        create_treasure(list, k, GT_INVISIBLE, 6, 0);
        if (check_treasure_arch(k, possible + 12, possible + 22)) {
            nastier++;
        }
    }
    fail_unless(nastier > 0, "should get a nastier trap");
}
END_TEST

static Suite *treasure_suite(void) {
    Suite *s = suite_create("treasure");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_add_treasure_in_list);
    tcase_add_test(tc_core, test_treasure_remove_item);
    tcase_add_test(tc_core, test_create_treasure_one);
    tcase_add_test(tc_core, test_create_treasure_all);
    tcase_add_test(tc_core, test_magic_limit);

    return s;
}

int main(void) {
    int nf;
    Suite *s = treasure_suite();
    SRunner *sr = srunner_create(s);


    /* to debug, uncomment this line */
    srunner_set_fork_status(sr, CK_NOFORK);

    srunner_set_xml(sr, LOGDIR "/unit/common/treasure.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/treasure.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
