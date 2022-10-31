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

#ifndef SOUND_FILES_H
#define SOUND_FILES_H

#include "../assets/AssetWrapper.h"

class SoundFile;

class SoundFiles : public AssetWrapper {
    Q_OBJECT

    Q_PROPERTY(QString licenseInformation READ licenseInformation)
public:
    SoundFiles(AssetWrapper *parent);

    virtual QString displayName() const override { return tr("Sound files"); }

    void refreshSounds();

    virtual int childrenCount() const override { return mySounds.size(); }
    virtual AssetWrapper *child(int index) override;
    virtual int childIndex(AssetWrapper *child) override;

    QString licenseInformation() const;

protected:
    std::vector<SoundFile *> mySounds;
};

#endif /* SOUND_FILES_H */
