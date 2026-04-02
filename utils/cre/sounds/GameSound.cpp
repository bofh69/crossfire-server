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

#include "GameSound.h"
#include "SoundFile.h"

GameSound::GameSound(AssetWrapper *parent, const QString &name, const QString &description)
 : AssetWrapper(parent, "GameSound"), myName(name), myDescription(description) {
}

AssetWrapper::PossibleUse GameSound::uses(const AssetWrapper *asset, std::string &) const {
    auto sound = dynamic_cast<const SoundFile *>(asset);
    if (sound) {
        bool uses = std::any_of(sound->events().cbegin(), sound->events().cend(), [this] (const std::string &event) {
            return event == myName.toStdString();
        });
        return uses ? Uses : DoesntUse;
    }
    return DoesntUse;
}
