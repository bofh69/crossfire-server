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

#ifndef GAME_SOUND_PANEL_H
#define GAME_SOUND_PANEL_H

#include "assets/AssetWrapperPanel.h"
#include "GameSound.h"

class GameSoundPanel : public AssetSWrapperPanel<GameSound> {
    Q_OBJECT
public:
    GameSoundPanel(QWidget *parent);

    virtual void updateItem() {}
};

#endif /* GAME_SOUND_PANEL_H */
