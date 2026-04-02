/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/*
 * This is the unit tests file for server/player.c
 */

#include <stdlib.h>
#include <global.h>
#include <check.h>
#include <loader.h>
#include <toolkit_common.h>
#include <sproto.h>

void setup(void) {
    settings.debug = llevError;
    cctk_setdatadir(SOURCE_ROOT "lib");
    init(0, NULL);
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST(test_get_nearest_player) {
    mapstruct *map = get_empty_map(5, 5);

    object *monster = create_archetype("kobold");
    FAIL_UNLESS(monster != NULL, "failed to find arch kobold");
    object_insert_in_map_at(monster, map, NULL, 0 , 0 , 0);

    object *fri = get_nearest_player(monster);
    FAIL_UNLESS(fri == NULL, "Shouldn't get any friend");

    // Add a player, should be found
    object *ob = create_archetype("angel");
    FAIL_UNLESS(QUERY_FLAG(ob, FLAG_MONSTER), "not a monster??");
    player pl;
    memset(&pl, 0, sizeof(pl));
    socket_struct sock;
    memset(&sock, 0, sizeof(sock));
    pl.socket = &sock;
    pl.ob = ob;
    first_player = &pl;
    object_insert_in_map_at(ob, map, NULL, 0, 4, 4);

    fri = get_nearest_player(monster);
    FAIL_UNLESS(fri == ob, "Should get the player");

    // Add a pet closer, should be found
    object *pet = create_archetype("vampire");
    FAIL_UNLESS(QUERY_FLAG(pet, FLAG_MONSTER), "not a monster??");
    SET_FLAG(pet, FLAG_FRIENDLY);
    add_friendly_object(pet);
    object_insert_in_map_at(pet, map, NULL, 0, 2, 2);
    fri = get_nearest_player(monster);
    FAIL_UNLESS(fri == pet, "Should find the pet");
}
END_TEST

Suite *player_suite(void) {
    Suite *s = suite_create("player");
    TCase *tc_core = tcase_create("Core");
    tcase_set_timeout(tc_core, 20);

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_get_nearest_player);

    return s;
}

int main(void) {
    int nf;
    Suite *s = player_suite();
    SRunner *sr = srunner_create(s);
//    srunner_set_fork_status (sr, CK_NOFORK);

    srunner_set_xml(sr, LOGDIR "/unit/server/player.xml");
    srunner_set_log(sr, LOGDIR "/unit/server/player.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
