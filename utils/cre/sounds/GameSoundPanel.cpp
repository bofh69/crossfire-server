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

#include "GameSoundPanel.h"

GameSoundPanel::GameSoundPanel(QWidget *parent) : AssetSWrapperPanel(parent) {
    addLabel(tr("Game sound:"), "displayName");
    addLabel(tr("Description:"), "description", true);
    addBottomFiller();
}
