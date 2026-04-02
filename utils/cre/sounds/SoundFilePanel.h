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

#ifndef SOUND_FILE_PANEL_H
#define SOUND_FILE_PANEL_H

#include "../assets/AssetWrapperPanel.h"
#include "SoundFile.h"

class QTreeWidget;
class AssetModel;

class SoundFilePanel : public AssetSWrapperPanel<SoundFile> {
    Q_OBJECT
public:
    SoundFilePanel(QWidget *parent, AssetModel *model);

    virtual void updateItem();

private:
    QTreeWidget *myLicense;
};

#endif /* SOUND_FILE_PANEL_H */
