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

#include "AttackMessagesWrapper.h"
#include "AttackMessagePanel.h"

static std::vector<const char *> types({
    "arrow",
    "drain",
    "electricity",
    "cold",
    "fire",
    "basic attack",
    "karate",
    "claw",
    "punch",
    "slash",
    "pierce",
    "cleave",
    "slice",
    "stab",
    "whip",
    "crush",
    "blud",
    "door",
    "poison and disease",
    "wraith feed",
});

AttackMessagesWrapper::AttackMessagesWrapper(AssetWrapper *parent) : AssetWrapper(parent) {
    setProperty(tipProperty, tr("Display all attack messages."));
    for (uint8_t type = 0; type < types.size(); type++) {
        myChildren.push_back(new SingleAttackWrapper(this, type, types[type]));
    }
}

int AttackMessagesWrapper::childIndex(AssetWrapper *asset) {
    for (size_t i = 0; i < myChildren.size(); i++) {
        if (asset == myChildren[i]) {
            return i;
        }
    }
    return -1;
}
