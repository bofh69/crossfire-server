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

#include "ArtifactsWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "faces/FaceWrapper.h"

ArtifactsWrapper::ArtifactsWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager)
 : AssetWrapper(parent), myResourcesManager(resourcesManager) {
    setProperty(tipProperty, tr("Display all artifacts."));
}

int ArtifactsWrapper::childrenCount() const {
    int count = 0;
    auto list = first_artifactlist;
    while (list) {
        count++;
        list = list->next;
    }
    return count;
}

AssetWrapper *ArtifactsWrapper::child(int index) {
    auto list = first_artifactlist;
    while (index > 0) {
        index--;
        list = list->next;
    }
    return myResourcesManager->wrap(list, this);
}

int ArtifactsWrapper::childIndex(AssetWrapper *child) {
    auto list = first_artifactlist;
    int index = 0;
    while (list) {
        if (myResourcesManager->wrap(list, this) == child) {
            return index;
        }
        list = list->next;
        index++;
    }
    return -1;
}

AssetWrapper::PossibleUse ArtifactsWrapper::uses(const AssetWrapper *asset, std::string &) const {
    if (dynamic_cast<const FaceWrapper *>(asset) || dynamic_cast<const AnimationWrapper *>(asset)) {
        return ChildrenMayUse;
    }
    return DoesntUse;
}
