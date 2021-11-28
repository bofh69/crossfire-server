#include "RecipePanel.h"
#include <Qt>
#include <QtWidgets>

#include "assets.h"
#include "AssetsManager.h"

#include "CREUtils.h"
#include "RecipeWrapper.h"

RecipePanel::RecipePanel(QWidget* parent) : AssetWrapperPanel(parent) {
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

void RecipePanel::setItem(AssetWrapper *asset) {
    AssetWrapperPanel::setItem(asset);

    auto recipe = dynamic_cast<RecipeWrapper *>(asset);
    Q_ASSERT(recipe);

    myArchetypes->clear();
    for (auto name : recipe->archs()) {
        auto arch = getManager()->archetypes()->find(name.toLocal8Bit().data());
        if (arch) {
            myArchetypes->addTopLevelItem(CREUtils::archetypeNode(arch, NULL));
        }
    }
    myIngredients->setPlainText(recipe->ingredients().join("\n"));
}
