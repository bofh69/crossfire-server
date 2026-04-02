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

#include "Animations.h"
#include "global.h"
#include "assets.h"

template<>
Animations *asset_create(const std::string& name) {
    Animations *anim = (Animations *)calloc(1, sizeof(Animations));
    anim->name = add_string(name.c_str());
    anim->num_animations = 1;
    return anim;
}

template<>
void asset_destroy(Animations *item) {
    free_string(item->name);
    free(item->faces);
    free(item);
}

AllAnimations::AllAnimations() {
    get("###none");
    m_undefined.clear();
}

void AllAnimations::replace(Animations *existing, Animations *update) {
    free(existing->faces);
    existing->faces = update->faces;
    update->faces = nullptr;
    existing->facings = update->facings;
    existing->num_animations = update->num_animations;
    existing->has_blank = false;
    for (uint8_t face = 0; existing->faces && face < existing->num_animations; face++)
        if (existing->faces[face] == blank_face) {
            existing->has_blank = true;
            break;
        }
    free_string(update->name);
    free(update);
}

void AllAnimations::added(Animations *anim) {
    anim->num = m_assets.size() - 1;
    anim->has_blank = false;
    for (uint8_t face = 0; anim->faces && face < anim->num_animations; face++)
        if (anim->faces[face] == blank_face) {
            anim->has_blank = true;
            break;
        }
}
