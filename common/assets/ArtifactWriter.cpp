/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020-2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "ArtifactWriter.h"

#include "string.h"
#include "Utils.h"

void ArtifactWriter::write(const artifact *item, StringBuffer *buf) {
    stringbuffer_append_string(buf, "Allowed");
    if (item->allowed.empty()) {
        stringbuffer_append_string(buf, " all");
    } else {
        char sep = ' ';
        for (const auto allowed : item->allowed) {
            stringbuffer_append_printf(buf, "%c%s", sep, allowed);
            sep = ',';
        }
    }
    stringbuffer_append_string(buf, "\n");

    if (item->chance) {
        stringbuffer_append_printf(buf, "chance %d\n", item->chance);
    }
    if (item->difficulty) {
        stringbuffer_append_printf(buf, "difficulty %d\n", item->difficulty);
    }
    if (item->item) {
        stringbuffer_append_printf(buf, "Object %s\n", item->item->name);
        auto name = item->item->name;
        item->item->name = nullptr;
        get_ob_diff(buf, item->item, &empty_archetype->clone);
        item->item->name = name;
        /** @todo check why not included in get_ob_diff */
        if (item->item->anim_speed) {
            stringbuffer_append_printf(buf, "anim_speed %uc\n", item->item->anim_speed);
        }

        auto inv = item->item->inv;
        while (inv) {
            stringbuffer_append_printf(buf, "arch %s\n", inv->arch->name);
            get_ob_diff(buf, inv, &inv->arch->clone);
            stringbuffer_append_string(buf, "end\n");
            inv = inv->below;
        }
        stringbuffer_append_string(buf, "end\n");
    }
}
