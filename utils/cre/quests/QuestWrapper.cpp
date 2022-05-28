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

#include "QuestWrapper.h"
#include "faces/FaceWrapper.h"

AssetWrapper::PossibleUse QuestWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) {
        return myWrappedItem->face == face->wrappedItem() ? Uses : DoesntUse;
    }
    return DoesntUse;
}
