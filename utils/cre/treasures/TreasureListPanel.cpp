#include <Qt>

extern "C" {
#include "global.h"
#include "treasure.h"
}

#include "TreasureListPanel.h"
#include "CREUtils.h"

#include "assets.h"
#include "AssetsManager.h"
#include "TreasureListWrapper.h"
#include "TreasureWrapper.h"

CRETreasurePanel::CRETreasurePanel(QWidget* parent) : AssetWrapperPanel(parent) {

    addCheckBox(tr("Single item"), "isSingleItem", false);

    myLayout->addWidget(new QLabel("Difficulty:"), 2, 0);
    myLayout->addWidget(myDifficulty = new QSpinBox(this), 2, 1);
    myDifficulty->setRange(0, 150);
    myDifficulty->setValue(150);
    QPushButton* generate = new QPushButton("generate", this);
    connect(generate, SIGNAL(clicked(bool)), this, SLOT(onGenerate(bool)));
    myLayout->addWidget(generate, 2, 2);
    myLayout->addWidget(myGenerated = new QTreeWidget(this), 3, 0, 1, 3);
    myGenerated->setHeaderLabel(tr("Generation result"));
    myGenerated->setIconSize(QSize(32, 32));

    myUsing = new QTreeWidget(this);
    myUsing->setColumnCount(1);
    myUsing->setHeaderLabel(tr("Used by"));
    myUsing->setIconSize(QSize(32, 32));
    myLayout->addWidget(myUsing, 1, 4, 3, 1);
}

void CRETreasurePanel::setItem(AssetWrapper *asset) {
    AssetWrapperPanel::setItem(asset);
    myTreasure = dynamic_cast<TreasureListWrapper *>(asset);
    Q_ASSERT(myTreasure);

    myUsing->clear();

    QTreeWidgetItem* root = NULL;

    QString name = myTreasure->displayName();

    getManager()->archetypes()->each([this, &root, &name] (const archetype * arch) {
        if (arch->clone.randomitems && name == arch->clone.randomitems->name) {
            if (root == NULL) {
                root = CREUtils::archetypeNode(NULL);
                myUsing->addTopLevelItem(root);
                root->setExpanded(true);
            }
            CREUtils::archetypeNode(arch, root);
        }
    });

    root = NULL;

    getManager()->treasures()->each([this, &root, &name] (treasurelist * list) {
        for (const treasure *t = list->items; t; t = t->next) {
            if (t->name == name) {
                if (root == NULL) {
                    root = CREUtils::treasureNode(NULL);
                    myUsing->addTopLevelItem(root);
                    root->setExpanded(true);
                }
                CREUtils::treasureNode(list, root);
            }
        }
    });

    myGenerated->clear();
}

void CRETreasurePanel::onGenerate(bool) {
    const int difficulty = myDifficulty->value();
    myGenerated->clear();
    object* result = object_new(), *item;
    create_treasure(const_cast<treasurelist *>(myTreasure->item()), result, 0, difficulty, 0);
    while ((item = result->inv)) {
        identify(result->inv);
        myGenerated->addTopLevelItem(CREUtils::objectNode(item, NULL));

        object_remove(item);
        object_free(item, 0);
    }

    object_free(result, 0);
}
