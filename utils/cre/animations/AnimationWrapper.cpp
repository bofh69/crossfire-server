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

#include "AnimationWrapper.h"
#include "../ResourcesManager.h"
#include "../CREPixmap.h"
#include "assets/AssetWrapperPanel.h"

AnimationWrapper::AnimationWrapper(AssetWrapper *parent, Animations *anim, ResourcesManager *resourcesManager)
 : AssetTWrapper(parent, "Animation", anim), myResourcesManager(resourcesManager) {
}

AssetWrapper::PossibleUse AnimationWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) {
        for (uint16_t f = 0; f < myWrappedItem->num_animations; f++) {
            if (face->wrappedItem() == myWrappedItem->faces[f]) {
                return Uses;
            }
        }
        return DoesntUse;
    }
    return DoesntUse;
}
