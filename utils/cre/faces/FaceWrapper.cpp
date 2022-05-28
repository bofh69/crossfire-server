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

#include "FaceWrapper.h"
#include "../ResourcesManager.h"
#include "../CREPixmap.h"

FaceWrapper::FaceWrapper(AssetWrapper *parent, Face *face, ResourcesManager *resourcesManager)
 : AssetTWrapper(parent, "Face", face), myResourcesManager(resourcesManager) {
}

AssetWrapper::PossibleUse FaceWrapper::uses(const AssetWrapper *asset, std::string &hint) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) {
        if (myWrappedItem->smoothface == face->myWrappedItem) {
            hint = "as smoothed face";
            return Uses;
        }
    }
    return DoesntUse;
}
