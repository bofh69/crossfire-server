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

#include "ArchetypePanel.h"

ArchetypePanel::ArchetypePanel(AssetModel *model, QWidget* parent) : AssetWrapperPanel(parent) {
    addTextEdit("Raw archetype:", "raw", false);
    addAssetUseTree(tr("Used by"), model, "self");
}
