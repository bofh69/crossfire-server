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

#include "RecipeWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "RecipeListWrapper.h"
#include "../archetypes/ArchetypeWrapper.h"

RecipeListWrapper::RecipeListWrapper(AssetWrapper *parent, const recipelist *list, ResourcesManager *resources)
  : AssetTWrapper(parent, "RecipeList", list), myResourcesManager(resources) {
}

int RecipeListWrapper::childrenCount() const {
    int count = 0;
    auto item = myWrappedItem->items;
    while (item) {
        count++;
        item = item->next;
    }
    return count;
}

AssetWrapper *RecipeListWrapper::child(int index) {
    auto item = myWrappedItem->items;
    while (index > 0) {
        index--;
        item = item->next;
    }
    return myResourcesManager->wrap(item, this);
}

int RecipeListWrapper::childIndex(AssetWrapper *child) {
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

AssetWrapper::PossibleUse RecipeListWrapper::uses(const AssetWrapper *asset, std::string &) const {
    return dynamic_cast<const ArchetypeWrapper *>(asset) ? ChildrenMayUse : DoesntUse;
}
