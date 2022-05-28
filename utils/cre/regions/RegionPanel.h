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

#ifndef REGION_PANEL_H
#define REGION_PANEL_H

#include <QWidget>
#include "assets/AssetWrapperPanel.h"

class RegionPanel : public AssetWrapperPanel {
public:
    RegionPanel(QWidget *parent);
};

#endif /* _CREREGIONPANEL_H */
