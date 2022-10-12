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

#ifndef QUESTS_WRAPPER_H
#define QUESTS_WRAPPER_H

#include "assets.h"
#include "AssetsManager.h"
#include "assets/AssetsCollectionWrapper.h"

/**
 * Asset wrapper representing all quests.
 */
class QuestsWrapper : public AssetsCollectionWrapper<quest_definition> {
    Q_OBJECT
public:
    QuestsWrapper(AssetWrapper *parent, ResourcesManager *resources) : AssetsCollectionWrapper(parent, tr("Quests"), getManager()->quests(), resources, tr("Display all quests.")) {
        myResources = resources;
    }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        if (dynamic_cast<const FaceWrapper *>(asset)) {
            return ChildrenMayUse;
        }
        return DoesntUse;
    }

    virtual void fillMenu(QMenu *menu) override;

protected:
    ResourcesManager *myResources;  /**< Resources manager, used in quest creation. */

    /**
     * Helper function to add a new quest.
     */
    void addQuest();
};

#endif /* QUESTS_WRAPPER_H */
