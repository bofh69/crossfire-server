/**
 * @file
 * Characters associated with an account.n
 */

#ifndef ACCOUNT_CHAR_H
#define ACCOUNT_CHAR_H

/**
 * One character in an account.
 */
typedef struct account_char_struct {
    sstring name;             /** < Name of this character/player */
    sstring character_class;  /** < Class of this character */
    sstring race;             /** < Race of this character */
    uint8_t level;            /** < Level of this character */
    sstring face;             /** < Face of this character */
    sstring party;            /** < Character this party belonged to */
    sstring map;              /** < Last map this character was on */
    uint8_t isDead;           /** < Should stay at zero if alive, anything else if dead (hopefully 1, but doesn't have to be) */
    struct account_char_struct  *next;
} Account_Char;

/**
 * Structure handling character information for an account.
 * Its fields should never be modified directly.
 */
typedef struct account_chars_struct {
    sstring account_name;   /**< Account the information is for. */
    uint8_t ref_count;      /**< Number of pointers on this structure. */
    Account_Char *chars;    /**< Characters of the account, in a linked list. */
} Account_Chars;

void account_char_remove(Account_Chars *chars, const char *pl_name);
int make_perma_dead(object *op);
int unmake_perma_dead(char *account, char *player);

#endif
