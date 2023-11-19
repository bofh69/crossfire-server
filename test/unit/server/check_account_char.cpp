/*
 * This is the unit tests file for server/account.c
 */

#include "global.h"

#include <check.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "account_char.h"
#include "loader.h"
#include "sproto.h"
#include "toolkit_common.h"
#include "toolkit_server.h"

static void setup(void) {
}

static void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}



/* This test tries to add some accounts.  As a final action, we try
 * to save the account file out.
 */
START_TEST(test_account_char_add) {
    player *pl;
    Account_Chars *chars;
    char path[MAX_BUF];

    pl =  static_cast<player *>(calloc(1, sizeof(player)));
    chars = account_char_load("testaccount");

    /* The account_character code takes a player structure to
     * fill in the values, so we create a fake one here -
     * we just fill in the fields that are used.
     */
    pl->ob = create_archetype("human_player");
    pl->ob->level = 1;
    pl->ob->name = add_string("test character");
    pl->ob->contr = pl;
    strcpy(pl->maplevel, "test map");

    account_char_add(chars, pl);
    FAIL_UNLESS(chars->chars.size() == 1, "account_char_add returned empty list on initial character");

    pl->ob->level = 2;
    account_char_add(chars, pl);
    FAIL_UNLESS(chars->chars.size() == 1, "account_char_add didn't on update character");

    account_char_remove(chars, pl->ob->name);
    FAIL_UNLESS(chars->chars.empty(), "account_char_remove returned non empty list on final character removal");

    account_char_add(chars, pl);
    FAIL_UNLESS(chars->chars.size() == 1, "account_char_add didn't insert initial character");

    pl->ob->name = add_string("char 2");
    pl->party = party_form(pl->ob, "rockon");

    account_char_add(chars, pl);
    FAIL_UNLESS(chars->chars.size() == 2, "account_char_add didn't add character");

    sprintf(path,"%s/account", settings.localdir);
    mkdir(path, S_IRWXU);

    /* This does not return anything, but this at least checks for
     * core dumps, etc
     */
    account_char_save(chars);

    /* Like above, this returns nothing but does check for core dumps */
    account_char_free(chars);
}
END_TEST

/* This tests the load logic.  Since the only data were are loading is the data from above,
 * we use that knowledge to check for proper existence accounts, etc.
 */
START_TEST(test_account_char_load) {
    Account_Chars *chars;
    object *ob = create_archetype("human_player");

    chars = account_char_load("testaccount");
    FAIL_UNLESS(chars != NULL, "account_char_load returned NULL");
    FAIL_UNLESS(chars->chars.size() == 2, "account_char_load didn't load the file");

    /* As of now, the account order is in FIFO order */

    FAIL_UNLESS(!strcmp(chars->chars[0]->name, "test character"),
                "Name for first character is not test char");

    FAIL_UNLESS(!strcmp(chars->chars[0]->race, ob->race),
                "Race for first character does not match");

    FAIL_UNLESS(chars->chars[0]->level == 2,
                "Level for first character is not 2");

    FAIL_UNLESS(!strcmp(chars->chars[0]->face, ob->face->name),
                "Face for first character does not match");

    FAIL_UNLESS(chars->chars[0]->party[0] == 0,
                "Party for first character is not blank");

    FAIL_UNLESS(!strcmp(chars->chars[0]->map, "test map"),
                "Map for first character does not match");

    /* The presumption here is that if it loaded the first entry
     * successfully, so it should the second, but we do check for the fields
     * which are different.
     */

    FAIL_UNLESS(!strcmp(chars->chars[1]->name, "char 2"),
                "Name for second character does not match");

    FAIL_UNLESS(!strcmp(chars->chars[1]->party, "rockon"),
                "Party for second character does not match");

    account_char_free(chars);
}

END_TEST

START_TEST(test_account_char_load_duplicate) {
    Account_Chars *first, *second;

    first = account_char_load("dummy");
    second = account_char_load("dummy");
    FAIL_UNLESS(first == second, "account_char_load should return the same structure for the same name");

    account_char_free(first);
    account_char_free(second);
}
END_TEST

static Suite *account_suite(void) {
    Suite *s = suite_create("account_char");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_account_char_add);
    tcase_add_test(tc_core, test_account_char_load);
    tcase_add_test(tc_core, test_account_char_load_duplicate);

    return s;
}

int main(void) {
    int nf;
    Suite *s = account_suite();
    SRunner *sr = srunner_create(s);

    /* If you wish to debug the program, uncomment this line. */
    /*srunner_set_fork_status (sr, CK_NOFORK);*/

    settings.debug = llevError;
    /* Not sure if there is a better place to put this file - basically,
     * the account code usings the localdir to determine where to read/write
     * the accounts file from - we don't want to be altering the real version of
     * that file.
     */
    settings.localdir = strdup_local("/tmp/");
    cctk_setdatadir(SOURCE_ROOT "lib");
    clean_test_account_data();
    init(0, NULL);

    srunner_set_xml(sr, LOGDIR "/unit/server/account_char.xml");
    srunner_set_log(sr, LOGDIR "/unit/server/account_char.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    clean_test_account_data();
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
