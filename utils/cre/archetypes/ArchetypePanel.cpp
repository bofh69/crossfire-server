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

#include <Qt>
#include "CREMapInformation.h"
#include "CREMapInformationManager.h"

extern "C" {
#include "global.h"
}

#include "ArchetypePanel.h"
#include "CREUtils.h"
#include "ResourcesManager.h"

#include "assets.h"
#include "AssetsManager.h"
#include "Archetypes.h"
#include "ArchetypeLoader.h"
#include "ResourcesManager.h"
#include "../assets/AssetModel.h"

ArchetypePanel::ArchetypePanel(AssetModel *model, QWidget* parent) : AssetWrapperPanel(parent) {
    addTextEdit("Raw archetype:", "raw", false);
    addAssetUseTree(tr("Used by"), model, "self");
}
