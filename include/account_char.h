/**
 * @file
 * Characters associated with an account.n
 */

#ifndef ACCOUNT_CHAR_H
#define ACCOUNT_CHAR_H

/**
 * One character in an account.
 */
struct Account_Char {
    sstring name;             /** < Name of this character/player */
    sstring character_class;  /** < Class of this character */
    sstring race;             /** < Race of this character */
    uint8_t level;            /** < Level of this character */
    sstring face;             /** < Face of this character */
    sstring party;            /** < Character this party belonged to */
    sstring map;              /** < Last map this character was on */
    uint8_t isDead;           /** < Should stay at zero if alive, anything else if dead (hopefully 1, but doesn't have to be) */
};

/**
 * Structure handling character information for an account.
 * Its fields should never be modified directly.
 */
struct Account_Chars {
    sstring account_name;   /**< Account the information is for. */
    uint8_t ref_count;      /**< Number of pointers on this structure. */
    std::vector<Account_Char *> chars;  /**< Characters of the account. */
};

void account_char_remove(Account_Chars *chars, const char *pl_name);
int make_perma_dead(object *op);
int unmake_perma_dead(char *account, char *player);

#endif
