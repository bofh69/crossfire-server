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

#include "Messages.h"
#include "global.h"

template<>
GeneralMessage *asset_create(const std::string& name) {
    auto msg = static_cast<GeneralMessage *>(calloc(1, sizeof(GeneralMessage)));
    msg->identifier = add_string(name.c_str());
    return msg;
}

template<>
void asset_destroy(GeneralMessage *item) {
    FREE_AND_CLEAR_STR_IF(item->identifier);
    FREE_AND_CLEAR_STR_IF(item->message);
    FREE_AND_CLEAR_STR_IF(item->quest_code);
    FREE_AND_CLEAR_STR_IF(item->title);
    free(item);
}

Messages::Messages() : m_totalChance(0) {
}

void Messages::replace(GeneralMessage *existing, GeneralMessage *update) {
    FREE_AND_COPY_IF(existing->identifier, update->identifier);
    FREE_AND_COPY_IF(existing->message, update->message);
    FREE_AND_COPY_IF(existing->quest_code, update->quest_code);
    FREE_AND_COPY_IF(existing->title, existing->title);
    m_totalChance -= existing->chance;
    m_totalChance += update->chance;
    existing->chance = update->chance;
    asset_destroy(update);
}

void Messages::added(GeneralMessage *asset) {
    m_totalChance += asset->chance;
}

GeneralMessage *Messages::random() {
    if (!m_totalChance) {
        return nullptr;
    }

    auto msg = m_assets.begin();
    int weight = (RANDOM() % m_totalChance);
    while (msg != m_assets.end()) {
        weight -= msg->second->chance;
        if (weight < 0) {
            return msg->second;
        }
        msg++;
    }
    return nullptr;
}
