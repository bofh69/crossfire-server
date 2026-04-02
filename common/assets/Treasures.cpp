/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "Treasures.h"

template<>
treasurelist *asset_create(const std::string& name) {
    treasurelist *tl = (treasurelist *)calloc(1, sizeof(treasurelist));
    tl->name = add_string(name.c_str());
    return tl;
}

template<>
void asset_destroy(treasurelist *item) {
    if (item->name)
        free_string(item->name);
    if (item->items)
        treasure_free(item->items);
    free(item);
}

void Treasures::replace(treasurelist *existing, treasurelist *update) {
    if (existing->items) {
        treasure_free(existing->items);
    }
    existing->items = update->items;
    existing->total_chance = update->total_chance;
    update->items = NULL;
    asset_destroy(update);
}
