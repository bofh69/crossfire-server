/**
 * test_mapsave.cpp -- save stability test
 *
 * This file creates a program that loads and saves a map given by the in-game
 * PATH (e.g. /world/world_105_115). If everything is working, load/save should
 * not change the file.
 *
 * Due to the lack of correct linked list reversing, we load/save twice to get
 * the same order of objects again.
 *
 * For normal (e.g. world) maps, load/save saves to a temporary file, so we can
 * avoid modifying the original file and diff between the original and saved
 * temporary file.
 *
 * For unique (e.g. apartment) maps, this test SAVES OVER THE ORIGINAL FILE. So
 * you should make a copy of the original file so you can diff the result.
 */

#include "global.h"

extern void map_path(const char *map, int flags, char *pathname, size_t bufsize);
extern mapstruct *mapfile_load_lowlevel(const char *map, const char *pathname, int flags);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("usage: test_mapsave PATH");
        return 0;
    }

    settings.debug = llevInfo;
    init_library();
    int flags = 0;
    if (argv[1][0] == '~') {
        flags |= MAP_PLAYER_UNIQUE;
    }

    // We have to do this twice because saving twice reverses the order of the
    // linked lists and so the save is "stable".
    mapstruct *m;
    m = mapfile_load(argv[1], flags);
    if (m == NULL) {
        return 1;
    }
    save_map(m, 0);

    // Save a copy of the original map's real path for diffing
    char buf[MAX_BUF];
    map_path(m->path, flags, buf, sizeof(buf));

    const char *path_firstpass = NULL;
    if (m->tmpname != NULL) {
        // happens if map is unique
        strdup(m->tmpname);
    } else {
        path_firstpass = buf;
    }
    free_map(m);
    m = mapfile_load_lowlevel(argv[1], path_firstpass, flags);
    if (m == NULL) {
        return 1;
    }
    save_map(m, 0);

    LOG(llevInfo, "Original:\t%s\n", buf);
    if (!(flags & MAP_PLAYER_UNIQUE)) {
        LOG(llevInfo, "Pass 1:\t%s\n", path_firstpass);
        LOG(llevInfo, "Pass 2:\t%s\n", m->tmpname);
    }
    free_map(m);
}
