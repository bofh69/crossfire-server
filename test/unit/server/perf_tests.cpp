/*
 * CrossFire, A Multiplayer game for X-windows
 *
 * Copyright (C) 2024 Crossfire Development Team
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
 * This is a performance-related test. You need to specify the number of loops to do on the command line.
 */

#include <stdlib.h>
#include <global.h>
#include <sproto.h>

#include <array>

constexpr int TestSize = 20;
int testCount = 0;

int legacy_monster_stand_in_light(object *op) {
    int16_t nx, ny;
    mapstruct *m;

    if (!op)
        return 0;
    if (op->glow_radius > 0)
        return 1;

    if (op->map) {
        int x, y;

        /* Check the spaces with the max light radius to see if any of them
         * have lights, and if any of them light the player enough, then return 1.
         */
        for (x = op->x-MAX_LIGHT_RADII; x <= op->x+MAX_LIGHT_RADII; x++) {
            for (y = op->y-MAX_LIGHT_RADII; y <= op->y+MAX_LIGHT_RADII; y++) {
                m = op->map;
                nx = x;
                ny = y;

                if (get_map_flags(m, &m, nx, ny, &nx, &ny)&P_OUT_OF_MAP)
                    continue;

                if (ihypot(x-op->x, y-op->y) < GET_MAP_LIGHT(m, nx, ny))
                    return 1;
            }
        }
    }
    return 0;
}

void test_stand_in_light() {
    pticks = 1;
    printf("running %d checks...\n", testCount);
    while (testCount-- > 0) {
        object dummy;
        memset(&dummy, 0, sizeof(dummy));
        mapstruct *test_maps[4];
        for (size_t i = 0; i < 4; i++)
            test_maps[i] = get_empty_map(TestSize, TestSize);
        test_maps[0]->tile_map[1] = test_maps[1];
        test_maps[0]->tile_map[2] = test_maps[2];
        test_maps[1]->tile_map[3] = test_maps[0];
        test_maps[1]->tile_map[2] = test_maps[3];
        test_maps[2]->tile_map[0] = test_maps[0];
        test_maps[2]->tile_map[1] = test_maps[3];
        test_maps[3]->tile_map[0] = test_maps[1];
        test_maps[3]->tile_map[3] = test_maps[2];

        for (int l = 40 + cf_random() % 25; l > 0; l--) {
            int map = cf_random() % 4, x = cf_random() % TestSize, y = cf_random() % TestSize, light = (1 + cf_random() % (MAX_LIGHT_RADII - 1));
            SET_MAP_LIGHT(test_maps[map], x, y, light);
        }

        for (int m = 0; m < 4; m++) {
            dummy.map = test_maps[m];
            for (dummy.x = 0; dummy.x < TestSize; dummy.x++) {
                for (dummy.y = 0; dummy.y < TestSize; dummy.y++) {
                    int old = legacy_monster_stand_in_light(&dummy);
                    int current = monster_stand_in_light(&dummy);
                    if (old != current) {
                        printf("%d %d?!?\n", old, current);
                    }
                    pticks = 1 - pticks;
                }
            }
        }

        for (size_t i = 0; i < 4; i++)
            delete_map(test_maps[i]);
    }
    printf("end\n");
}

int main(int argc, const char **argv) {
    if (argc > 1 && ((testCount = atoi(argv[1])) > 0))
        test_stand_in_light();
    return 0;
}
