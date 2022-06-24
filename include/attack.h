/**
 * @file
 * Attack-related definitions.
 */

#ifndef ATTACK_H
#define ATTACK_H

/**
 * @defgroup Attacktypes Attack types
 * - ATNR_xxx is the attack number that is indexed into the
 * the resist array in the object structure.
 * - ATM_xxx is the message number
 * - AT_xxx is the bitmask in the obj::attacktype field.
 */
/*@{*/
#define NROFATTACKS             26
#define NROFATTACKMESS          21
#define MAXATTACKMESS           21

/* attack message numbers must be less than NROFATTACKMESS */

#define ATM_ARROW       0
#define ATM_DRAIN       1
#define ATM_ELEC        2
#define ATM_COLD        3
#define ATM_FIRE        4
#define ATM_BASIC       5
#define ATM_KARATE      6
#define ATM_CLAW        7
#define ATM_PUNCH       8
#define ATM_SLASH       9
#define ATM_PIERCE      10
#define ATM_CLEAVE      11
#define ATM_SLICE       12
#define ATM_STAB        13
#define ATM_WHIP        14
#define ATM_CRUSH       15
#define ATM_BLUD        16
#define ATM_DOOR        17
#define ATM_SUFFER      18
#define ATM_WRAITH_FEED 19

/* Note that the last ATNR_ should be one less than NROFATTACKS above
 * since the ATNR starts counting at zero.
 * For compatible loading, these MUST correspond to the same value
 * as the bitmasks below.
 */
#define ATNR_PHYSICAL           0
#define ATNR_MAGIC              1
#define ATNR_FIRE               2
#define ATNR_ELECTRICITY        3
#define ATNR_COLD               4
#define ATNR_CONFUSION          5
#define ATNR_ACID               6
#define ATNR_DRAIN              7
#define ATNR_WEAPONMAGIC        8
#define ATNR_GHOSTHIT           9
#define ATNR_POISON             10
#define ATNR_SLOW               11
#define ATNR_PARALYZE           12
#define ATNR_TURN_UNDEAD        13
#define ATNR_FEAR               14
#define ATNR_CANCELLATION       15
#define ATNR_DEPLETE            16
#define ATNR_DEATH              17
#define ATNR_CHAOS              18
#define ATNR_COUNTERSPELL       19
#define ATNR_GODPOWER           20
#define ATNR_HOLYWORD           21
#define ATNR_BLIND              22
#define ATNR_INTERNAL           23
#define ATNR_LIFE_STEALING      24
#define ATNR_DISEASE            25

#define AT_PHYSICAL     0x00000001 /*       1 */
#define AT_MAGIC        0x00000002 /*       2 */
#define AT_FIRE         0x00000004 /*       4 */
#define AT_ELECTRICITY  0x00000008 /*       8 */
#define AT_COLD         0x00000010 /*      16 */
#define AT_CONFUSION    0x00000020 /*      32 The spell will use this one */
#define AT_ACID         0x00000040 /*      64 Things might corrode when hit */
#define AT_DRAIN        0x00000080 /*     128 */
#define AT_WEAPONMAGIC  0x00000100 /*     256 Very special, use with care */
#define AT_GHOSTHIT     0x00000200 /*     512 Attacker dissolves */
#define AT_POISON       0x00000400 /*    1024 */
#define AT_SLOW         0x00000800 /*    2048 */
#define AT_PARALYZE     0x00001000 /*    4096 */
#define AT_TURN_UNDEAD  0x00002000 /*    8192 */
#define AT_FEAR         0x00004000 /*   16384 */
#define AT_CANCELLATION 0x00008000 /*   32768 ylitalo@student.docs.uu.se */
#define AT_DEPLETE      0x00010000 /*   65536 vick@bern.docs.uu.se */
#define AT_DEATH        0x00020000 /*  131072 peterm@soda.berkeley.edu */
#define AT_CHAOS        0x00040000 /*  262144 peterm@soda.berkeley.edu*/
#define AT_COUNTERSPELL 0x00080000 /*  524288 peterm@soda.berkeley.edu*/
#define AT_GODPOWER     0x00100000 /* 1048576  peterm@soda.berkeley.edu */
#define AT_HOLYWORD     0x00200000 /* 2097152 race selective attack thomas@astro.psu.edu */
#define AT_BLIND        0x00400000 /* 4194304 thomas@astro.psu.edu */
#define AT_INTERNAL     0x00800000 /* Only used for internal calculations */
#define AT_LIFE_STEALING \
                        0x01000000 /* 16777216 for hp drain */
#define AT_DISEASE      0x02000000 /* 33554432 disease attacktypes */
/*@}*/

/* attacktypes_load is suffixed to resist_ when saving objects.
 * (so the line may be 'resist_fire' 20 for example).  These are never
 * seen by the player.  loader.l uses the same names, but it doesn't look
 * like you can use these values, so in that function they are hard coded.
 */

/* Note that internal should not ever be referanced in the last two
 * tables.  however, other resisttypes may be added, and if through some
 * bug these do get used somehow, might as well make it more easier to notice
 * and not have mystery values appear.
 */

/** Attack messages structure. */
typedef struct attackmess {
    int level;
    char *buf1;
    char *buf2;
    char *buf3;
} attackmess_t;

typedef struct {
    int attacktype;
    int face;
} Chaos_Attacks;

/** Attack messages the player gets when hitting/getting hit. */
EXTERN attackmess_t attack_mess[NROFATTACKMESS][MAXATTACKMESS];

EXTERN Chaos_Attacks ATTACKS[22];
EXTERN const char *const change_resist_msg[NROFATTACKS];
EXTERN const char *const resist_plus[NROFATTACKS];
EXTERN const char *const resist_color[NROFATTACKS];
EXTERN const char *const attacktype_desc[NROFATTACKS];
EXTERN const char *const resist_save[NROFATTACKS];

#endif /* ATTACK_H */
