/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "global.h"
#include "assets.h"
#include "AssetsManager.h"

#include <stdlib.h>

/**
 * @file
 * The functions in this file are purely mean to generate information
 * in differently formatted output, mainly about monsters.
 */

/**
 * Writes num ones and zeros to standard output based on the
 * bits variable.
 *
 * @param bits
 * variable to convert to binary string
 * @param num
 * number of bits to dump. Values above 32 will be ignored.
 */
static void bitstostring(long bits, int num) {
    if (num > 32)
        num = 32;

    for (int i = 0; i < num; i++) {
        if (bits&1)
            putchar('1');
        else
            putchar('0');
        putchar(',');
        bits >>= 1;
    }
}

/**
 * Dump to standard out the abilities of all monsters.
 */
void dump_abilities(void) {
    getManager()->archetypes()->each([] (const auto at) {
        const char *gen_name = "";
        archetype *gen;

        if (!QUERY_FLAG(&at->clone, FLAG_MONSTER))
            return;

        /* Get rid of e.g. multiple black puddings */
        if (QUERY_FLAG(&at->clone, FLAG_CHANGING))
            return;

        /* Dont print a row for each piece of a multipart object
         * Doing so confounds the documentation generators that use the -m2 flag.
         */
        if (HEAD(&at->clone) != &at->clone)
            return;

        gen = getManager()->archetypes()->first([&at] (const auto gen) {
            return gen->clone.other_arch && gen->clone.other_arch == at;
        });
        if (gen) {
            gen_name = gen->name;
        }

        char *name = stringbuffer_finish(describe_item(&at->clone, NULL, 0, NULL));
        printf("%-16s|%6" FMT64 "|%4d|%3d|%s|%s|%s\n", at->clone.name, at->clone.stats.exp,
               at->clone.stats.hp, at->clone.stats.ac, name, at->name, gen_name);
        free(name);
    });
}

/**
 * As dump_abilities(), but with an alternative way of output.
 */
void print_monsters(void) {
    // Generate CSV header
    printf("monster,hp,dam,ac,wc,");
    for (int i = 0; i < NROFATTACKS; i++) {
        printf("%s,", attacktype_desc[i]);
    }
    for (int i = 0; i < NROFATTACKS; i++) {
        printf("%s,", resist_plus[i]);
    }
    printf("exp,new exp\n");

    // Monster data
    getManager()->archetypes()->each([] (const auto at) {
        object *op = arch_to_object(at);
        if (QUERY_FLAG(op, FLAG_MONSTER)) {
            printf("%s,%d,%d,%d,%d,",
                   op->arch->name, op->stats.maxhp, op->stats.dam, op->stats.ac,
                   op->stats.wc);
            bitstostring((long)op->attacktype, NROFATTACKS);
            for (int i = 0; i < NROFATTACKS; i++)
                printf("%d,", op->resist[i]);
            printf("%" FMT64 ",%" FMT64 "\n", op->stats.exp, new_exp(op));
        }
        object_free_drop_inventory(op);
    });
}
