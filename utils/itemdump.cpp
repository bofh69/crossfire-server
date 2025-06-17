/**
 * itemdump -- load a map and dump item values and other information to CSV
 * Useful for looking at what people sold to pawn shops, have in apartments, etc.
 */
#include "global.h"
#include "shop.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("usage: shopitems PATH");
        return 0;
    }

    init_library();
    int flags = 0;
    if (argv[1][0] == '~') {
        flags |= MAP_PLAYER_UNIQUE;
    }
    mapstruct *m = ready_map_name(argv[1], flags); // instead of mapfile_load to get unique items
    if (m == NULL) {
        fprintf(stderr, "failed to load map\n");
        return 1;
    }

    // move everything to (0, 0) to try to get stackable things to stack
    for (int x = 0; x < m->width; x++) {
        for (int y = 0; y < m->height; y++) {
        FOR_MAP_PREPARE(m, x, y, ob) {
            if (QUERY_FLAG(ob, FLAG_NO_PICK))
                continue;
            if (QUERY_FLAG(ob, FLAG_IS_FLOOR))
                break;
            object_remove(ob);
            object_insert_in_map_at(ob, ob->map, ob, 0, 0, 0);
        } FOR_MAP_FINISH();
        }
    }

    FOR_MAP_PREPARE(m, 0, 0, ob) {
        if (QUERY_FLAG(ob, FLAG_NO_PICK))
            continue;
        if (QUERY_FLAG(ob, FLAG_IS_FLOOR))
            break;
        char buf[MAX_BUF];
        uint64_t base = price_base(ob);
        int enc = calc_item_enhancement(ob);
        query_base_name(ob, 0, buf, sizeof(buf));
        printf("%d,\"%s\",%d,%d,%lu\n", NROF(ob), buf, ob->value, enc, base);
    } FOR_MAP_FINISH();
}
