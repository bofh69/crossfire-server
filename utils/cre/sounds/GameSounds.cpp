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

#include "GameSounds.h"
#include "GameSound.h"
#include "SoundFile.h"
#include <QVariant>

std::map<QString, std::string> GameSounds::gameSounds = {
    {"learn", QT_TR_NOOP("player learns a spell")},
    {"apply", QT_TR_NOOP("player applies an item")},
    // evaporate may be an item evaporating when polymorphed, but the spell is disabled
    {"evaporate", QT_TR_NOOP("someone applies an item that is only available to DM; an item is destroyed to save the player's life")},
    {"hit", QT_TR_NOOP("player (or pet) hits a living creature")},
    {"miss", QT_TR_NOOP("player fails hitting a living creature")},
    {"kill", QT_TR_NOOP("player is killed")},
    {"step", QT_TR_NOOP("player is walking")},
    {"fire", QT_TR_NOOP("player fires a bow or crossbow")},
    {"poof", QT_TR_NOOP("player attempts to use a rod or wand without enough sp or charges")},
    {"open", QT_TR_NOOP("player opens a door with a key")},
    {"push", QT_TR_NOOP("a living creature is pushed")},
    {"death", QT_TR_NOOP("player dies")},
    {"explode", QT_TR_NOOP("a wand explodes when an attempt to recharged it is made")},
    {"fumble", QT_TR_NOOP("player fails casting a praying spell; player fails to learn a spell")},
    {"button click", QT_TR_NOOP("a button is walked on")},
    {"turn handle", QT_TR_NOOP("a handle is applied")},
    {"tick", QT_TR_NOOP("player applies a clock")},
    {"drink", QT_TR_NOOP("someone drinks an item")},
    {"gate moving", QT_TR_NOOP("a gate is opening or closing")},
    {"fall hole", QT_TR_NOOP("someone falls in a hole")},
    {"poison", QT_TR_NOOP("player applies a poisoned food or drink")},
};

GameSounds::GameSounds(AssetWrapper *parent) : AssetWrapper(parent, "GameSounds") {
    setProperty(tipProperty, tr("Display all game internal sounds."));
    for (const auto &sound : gameSounds) {
        mySounds.push_back(new GameSound(this, tr(sound.first.toStdString().data()), tr(sound.second.data())));
    }
}

AssetWrapper::PossibleUse GameSounds::uses(const AssetWrapper *asset, std::string &) const {
    return dynamic_cast<const SoundFile *>(asset) ? ChildrenMayUse : DoesntUse;
}

QString GameSounds::description() const {
    return tr("All sound names defined in the server and sent to the client");
}

AssetWrapper *GameSounds::child(int child) {
    return mySounds[child];
}

int GameSounds::childIndex(AssetWrapper *child) {
    auto pos = std::find(mySounds.cbegin(), mySounds.cend(), child);
    return pos == mySounds.cend() ? -1 : pos - mySounds.cbegin();
}
