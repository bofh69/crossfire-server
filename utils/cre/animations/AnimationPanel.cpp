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

extern "C" {
#include "global.h"
#include "artifact.h"
#include "object.h"
#include "face.h"
}

#include "AnimationPanel.h"
#include "AnimationControl.h"
#include "CREUtils.h"

#include "assets.h"
#include "AssetsManager.h"
#include "Archetypes.h"
#include "CREMapInformationManager.h"
#include "assets/AssetModel.h"
#include "animations/AnimationWrapper.h"

AnimationPanel::AnimationPanel(QWidget* parent, AssetModel *assets) : AssetTWrapperPanel(parent) {
    addAssetUseTree("Used by", assets, "self");

    myFaces = addWidget("", new QTreeWidget(this), false, nullptr, nullptr);
    myFaces->setColumnCount(1);
    myFaces->setHeaderLabel(tr("Faces"));
    myFaces->setIconSize(QSize(32, 32));

    myDisplay = addWidget("", new AnimationControl(this), false, nullptr, nullptr);
}

void AnimationPanel::updateItem() {
    myDisplay->setAnimation(myItem);

    myFaces->clear();
    auto root = CREUtils::faceNode(NULL);
    myFaces->addTopLevelItem(root);
    root->setExpanded(true);

    for (int face = 0; face < myItem->num_animations; face++)
    {
      CREUtils::faceNode(myItem->faces[face], root);
    }
}
