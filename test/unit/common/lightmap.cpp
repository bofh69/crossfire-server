#include <stdio.h>

#include "global.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("usage: lightmap PATH");
        return 0;
    }

    init_library();
    int flags = 0;
    if (argv[1][0] == '~') {
        flags |= MAP_PLAYER_UNIQUE;
    }

    mapstruct *m = mapfile_load(argv[1], flags);
    if (m == NULL) {
        fprintf(stderr, "failed to load map\n");
        return 1;
    }

    // light map (radius on each tile)
    FILE *orig = fopen("1.pgm", "w");
    fprintf(orig, "P2\n%d %d\n%d\n", m->width, m->height, 8); // blocked (0) or light radius + 1
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            MapSpace *s = map_space(m, x, y);
            SET_MAP_FLAGS(m, x, y, P_NEED_UPDATE);
            update_position(m, x, y);
            fprintf(orig, "%d ", GET_MAP_FLAGS(m, x, y) & P_BLOCKSVIEW ? 0 : s->light + 1);
        }
        fprintf(orig, "\n");
    }
    fclose(orig);

    // actual light map after spreading
    FILE *spread = fopen("2.pgm", "w");
    fprintf(spread, "P2\n%d %d\n%d\n", m->width, m->height, 2); // blocked (0), unlit (1), lit (2)
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            fprintf(spread, "%d ", GET_MAP_FLAGS(m, x, y) & P_BLOCKSVIEW ? 0 : map_light_on(m, x, y) + 1);
        }
        fprintf(spread, "\n");
    }
    fclose(spread);
}
