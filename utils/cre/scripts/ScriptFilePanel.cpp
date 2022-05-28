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

#include <QtWidgets>
#include "ScriptFilePanel.h"
#include "ScriptFile.h"
#include "ScriptFile.h"
#include "CREMapInformation.h"
#include "CREPixmap.h"

extern "C" {
#include "define.h"
}
#include "assets.h"
#include "AssetsManager.h"

CREScriptPanel::CREScriptPanel(AssetModel *assets, QWidget* parent) : AssetWrapperPanel(parent)
{
    addAssetUseTree(tr("Maps using this script"), assets, "self");
}
