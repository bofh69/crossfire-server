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

#ifndef SOUND_FILE_H
#define SOUND_FILE_H

#include "../assets/AssetWrapper.h"
#include "../LicenseManager.h"

class SoundFile : public AssetWrapper {
public:
    SoundFile(const std::vector<std::string> &events, AssetWrapper *parent, const QString &name);

    virtual QString displayName() const override { return myName; }

    std::vector<LicenseManager::LicenseItem> &license() { return myLicense; }

    const std::vector<std::string> &events() const { return myEvents; }

private:
    QString myName;
    std::vector<LicenseManager::LicenseItem> myLicense;
    std::vector<std::string> myEvents;
};

#endif /* SOUNDFILE_H */

