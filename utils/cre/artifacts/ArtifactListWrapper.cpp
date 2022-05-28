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

#include "ArtifactListWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "faces/FaceWrapper.h"

ArtifactListWrapper::ArtifactListWrapper(AssetWrapper *parent, artifactlist *list, ResourcesManager *resourcesManager)
 : AssetTWrapper(parent, "ArtifactList", list), myResourcesManager(resourcesManager) {
}

int ArtifactListWrapper::childrenCount() const {
    int count = 0;
    auto item = myWrappedItem->items;
    while (item) {
        count++;
        item = item->next;
    }
    return count;
}

AssetWrapper *ArtifactListWrapper::child(int index) {
    auto item = myWrappedItem->items;
    while (index > 0) {
        index--;
        item = item->next;
    }
    return myResourcesManager->wrap(item, this);
}

int ArtifactListWrapper::childIndex(AssetWrapper *child) {
    auto item = myWrappedItem->items;
    int index = 0;
    while (item) {
        if (myResourcesManager->wrap(item, this) == child) {
            return index;
        }
        item = item->next;
        index++;
    }
    return -1;
}

AssetWrapper::PossibleUse ArtifactListWrapper::uses(const AssetWrapper *asset, std::string &) const {
    if (dynamic_cast<const FaceWrapper *>(asset) || dynamic_cast<const AnimationWrapper *>(asset)) {
        return ChildrenMayUse;
    }
    return DoesntUse;
}

void ArtifactListWrapper::wasModified(AssetWrapper *asset, ChangeType type, int extra) {
    if (childIndex(asset) != -1 && type == AssetUpdated) {
        myWrappedItem->total_chance = 0;
        auto item = myWrappedItem->items;
        while (item) {
            myWrappedItem->total_chance += item->chance;
            item = item->next;
        }
        markModified(AssetUpdated, 1);
        return;
    }
    AssetWrapper::wasModified(asset, type, extra);
}
