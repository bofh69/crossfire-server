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

    const char *path_firstpass = strdup(m->tmpname);
    free_map(m);
    m = mapfile_load_lowlevel(argv[1], path_firstpass, flags);
    if (m == NULL) {
        return 1;
    }
    save_map(m, 0);

    LOG(llevInfo, "Original:\t%s\n", buf);
    LOG(llevInfo, "Pass 1:\t%s\n", path_firstpass);
    LOG(llevInfo, "Pass 2:\t%s\n", m->tmpname);
    free_map(m);
}
