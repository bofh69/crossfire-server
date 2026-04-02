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

#include "global.h"
#include "treasure.h"

#include "TreasureListPanel.h"
#include "CREUtils.h"

#include "assets.h"
#include "AssetsManager.h"
#include "TreasureListWrapper.h"
#include "TreasureWrapper.h"
#include "../assets/AssetUseTree.h"
#include "assets/AssetModel.h"

CRETreasurePanel::CRETreasurePanel(AssetModel* model, QWidget* parent) : AssetTWrapperPanel(parent) {

    addCheckBox(tr("Single item"), "isSingleItem", false);

    myLayout->addWidget(new QLabel(tr("Difficulty:")), 2, 0);
    myLayout->addWidget(myDifficulty = new QSpinBox(this), 2, 1);
    myDifficulty->setRange(0, 150);
    myDifficulty->setValue(150);
    QPushButton* generate = new QPushButton(tr("generate"), this);
    connect(generate, SIGNAL(clicked(bool)), this, SLOT(onGenerate(bool)));
    myLayout->addWidget(generate, 2, 2);
    myLayout->addWidget(myGenerated = new QTreeWidget(this), 3, 0, 1, 3);
    myGenerated->setHeaderLabel(tr("Generation result"));
    myGenerated->setIconSize(QSize(32, 32));

    myLayout->addWidget(new QLabel(tr("Used by"), this), 1, 4, 1, 1);
    auto use = addAssetUseTree(QString(), model, "self");
    myLayout->removeWidget(use);
    myLayout->addWidget(use, 2, 4, 2, 3);
}

void CRETreasurePanel::updateItem() {
    myGenerated->clear();
}

void CRETreasurePanel::onGenerate(bool) {
    const int difficulty = myDifficulty->value();
    myGenerated->clear();
    object* result = object_new(), *item;
    create_treasure(const_cast<treasurelist *>(myItem), result, 0, difficulty, 0);
    while ((item = result->inv)) {
        identify(result->inv);
        myGenerated->addTopLevelItem(CREUtils::objectNode(item, NULL));

        object_remove(item);
        object_free(item, 0);
    }

    object_free(result, 0);
}
