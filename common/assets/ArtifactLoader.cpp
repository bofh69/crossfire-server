/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "ArtifactLoader.h"
#include "Utils.h"
#include "global.h"
#include "artifact.h"

bool ArtifactLoader::willLoad(const std::string &filename) {
    return
            Utils::endsWith(filename, "/artifacts")
            || Utils::endsWith(filename, ".artifacts");
}

void ArtifactLoader::load(BufferReader *reader, const std::string &filename) {
    char *buf, *cp, *next;
    artifact *art = NULL;
    int value;
    artifactlist *al;
    archetype dummy_archetype;

    memset(&dummy_archetype, 0, sizeof(archetype));

    while ((buf = bufferreader_next_line(reader)) != NULL) {
        if (*buf == '#')
            continue;
        cp = buf;
        while (*cp == ' ') /* Skip blanks */
            cp++;
        if (*cp == '\0')
            continue;

        if (!strncmp(cp, "Allowed", 7)) {
            if (art == NULL) {
                art = get_empty_artifact();
                nrofartifacts++;
            }

            cp = strchr(cp, ' ')+1;
            while (*(cp+strlen(cp)-1) == ' ')
                cp[strlen(cp)-1] = '\0';

            if (!strcmp(cp, "all"))
                continue;

            do {
                while (*cp == ' ')
                    cp++;
                nrofallowedstr++;
                if ((next = strchr(cp, ',')) != NULL)
                    *(next++) = '\0';
                art->allowed.push_back(add_string(cp));
            } while ((cp = next) != NULL);
        } else if (sscanf(cp, "chance %d", &value) && art)
            art->chance = (uint16_t)value;
        else if (sscanf(cp, "difficulty %d", &value) && art)
            art->difficulty = (uint8_t)value;
        else if (!strncmp(cp, "Object", 6) && art) {
            art->item = (object *)calloc(1, sizeof(object));
            if (art->item == NULL) {
                LOG(llevError, "init_artifacts: memory allocation failure.\n");
                abort();
            }
            object_reset(art->item);
            art->item->arch = &dummy_archetype;
            if (!load_object_from_reader(reader, art->item, MAP_STYLE, false, true))
                LOG(llevError, "Init_Artifacts: Could not load object.\n");
            art->item->arch = NULL;
            art->item->name = add_string((strchr(cp, ' ')+1));
            al = find_artifactlist(art->item->type);
            if (al == NULL) {
                al = get_empty_artifactlist();
                al->type = art->item->type;
                al->next = first_artifactlist;
                first_artifactlist = al;
            }
            al->items.push_back(art);
            if (myTracker) {
                myTracker->assetDefined(art, filename);
            }
            art = NULL;
        } else
            LOG(llevError, "Unknown input in artifact file %s:%zu: %s\n", filename.c_str(), bufferreader_current_line(reader), buf);
    }

    for (al = first_artifactlist; al != NULL; al = al->next) {
        al->total_chance = 0;
        for (auto art : al->items) {
            if (!art->chance)
                LOG(llevDebug, "Artifact with no chance: %s\n", art->item->name);
            else
                al->total_chance += art->chance;
        }
#if 0
        LOG(llevDebug, "Artifact list type %d has %d total chance\n", al->type, al->total_chance);
#endif
    }
}
