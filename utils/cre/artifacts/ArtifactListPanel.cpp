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

#include "global.h"
#include "artifact.h"

#include "ArtifactListPanel.h"
#include "CREUtils.h"
#include "animations/AnimationWidget.h"

#include "assets.h"
#include "AssetsManager.h"
#include "Archetypes.h"

ArtifactListPanel::ArtifactListPanel(QWidget *parent) : AssetTWrapperPanel(parent) {
}

void ArtifactListPanel::updateItem() {
}
