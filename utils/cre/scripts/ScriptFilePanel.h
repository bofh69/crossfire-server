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

#ifndef SCRIPT_FILE_PANEL_H
#define SCRIPT_FILE_PANEL_H

#include "assets/AssetWrapperPanel.h"

class AssetModel;

/**
 * Display details of a Python script.
 */
class CREScriptPanel : public AssetWrapperPanel {
    Q_OBJECT
public:
    CREScriptPanel(AssetModel *assets, QWidget *parent);
};

#endif /* SCRIPT_FILE_PANEL_H */
