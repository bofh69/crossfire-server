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

class ResourcesManager;

class TreasuresWrapper : public AssetsCollectionWrapper<treasurelist> {
    Q_OBJECT
public:
    TreasuresWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager) : AssetsCollectionWrapper(parent, tr("Treasures"), getManager()->treasures(), resourcesManager, tr("Display all treasures.")) {
    }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        return dynamic_cast<const ArchetypeWrapper *>(asset) || dynamic_cast<const TreasureListWrapper *>(asset) ? ChildrenMayUse : DoesntUse;
    }
};

#endif /* TREASURES_WRAPPER_H */
