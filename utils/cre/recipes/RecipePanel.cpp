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

#include "RecipePanel.h"
#include <Qt>
#include <QtWidgets>

#include "assets.h"
#include "AssetsManager.h"

#include "CREUtils.h"
#include "RecipeWrapper.h"

RecipePanel::RecipePanel(QWidget* parent) : AssetTWrapperPanel(parent) {
    addLineEdit(tr("Title:"), "title");
    addLineEdit(tr("Skill:"), "skill");
    addLineEdit(tr("Cauldron:"), "cauldron");
    addLineEdit(tr("Yield:"), "yield");
    addLineEdit(tr("Chance:"), "chance");
    addLineEdit(tr("Experience:"), "experience");
    addLineEdit(tr("Difficulty:"), "difficulty");
    addCheckBox(tr("Transmutation"), "transmute");
    addLineEdit(tr("Index:"), "index");
    myIngredients = addTextEdit(tr("Ingredients:"), nullptr);

    myArchetypes = new QTreeWidget(this);
    myArchetypes->setHeaderLabel(tr("Archetypes:"));
    myArchetypes->setRootIsDecorated(false);
    myArchetypes->setIconSize(QSize(32, 32));
    myLayout->addWidget(myArchetypes, myLayout->rowCount(), 0, 1, 2);
}

void RecipePanel::updateItem() {
    myArchetypes->clear();
    auto rec = dynamic_cast<RecipeWrapper *>(myAsset);
    for (auto name : rec->archs()) {
        auto arch = getManager()->archetypes()->find(name.toLocal8Bit().data());
        if (arch) {
            myArchetypes->addTopLevelItem(CREUtils::archetypeNode(arch, NULL));
        }
    }
    myIngredients->setPlainText(rec->ingredients().join("\n"));
}
