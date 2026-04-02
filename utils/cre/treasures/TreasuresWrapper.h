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

#ifndef TREASURES_WRAPPER_H
#define TREASURES_WRAPPER_H

#include "global.h"

#include "AssetsManager.h"
#include "assets.h"
#include "../assets/AssetsCollectionWrapper.h"
#include "../archetypes/ArchetypeWrapper.h"
#include "TreasureListWrapper.h"
#include "../assets/AssetOriginAndCreationDialog.h"

class ResourcesManager;

class TreasuresWrapper : public AssetsCollectionWrapper<treasurelist> {
    Q_OBJECT
public:
    TreasuresWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager)
        : AssetsCollectionWrapper(parent, tr("Treasures"), getManager()->treasures(), resourcesManager,
             tr("Display all treasures.")), myResources(resourcesManager) {
    }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        return dynamic_cast<const ArchetypeWrapper *>(asset) || dynamic_cast<const TreasureListWrapper *>(asset) ? ChildrenMayUse : DoesntUse;
    }

    void fillMenu(QMenu *menu) {
        connect(menu->addAction(tr("Add treasure list")), &QAction::triggered, [this] () { addTreasureList(); });
    }

protected:
    void addTreasureList() {
        auto origins = myResources->treasureFiles();
        auto keys = getManager()->treasures()->keys();

        AssetOriginAndCreationDialog aqd(AssetOriginAndCreationDialog::Treasure, AssetOriginAndCreationDialog::CreateAsset, "", origins, keys);
        if (aqd.exec() != QDialog::Accepted) {
            return;
        }
        auto list = asset_create<treasurelist>(aqd.code().toStdString());
        getManager()->treasures()->define(list->name, list);
        myResources->assetDefined(list, aqd.file().toStdString());
        myResources->treasureModified(list);
        markModified(BeforeChildAdd, myAssets.size());
        myAssets.push_back(myResources->wrap(list, this));
        markModified(AfterChildAdd, myAssets.size());
    }

    ResourcesManager *myResources;
};

#endif /* TREASURES_WRAPPER_H */
