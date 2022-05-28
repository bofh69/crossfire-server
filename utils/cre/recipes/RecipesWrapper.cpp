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

#include "RecipesWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "artifacts/ArtifactsWrapper.h"
#include "../archetypes/ArchetypeWrapper.h"

RecipesWrapper::RecipesWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager)
 : AssetWrapper(parent), myResourcesManager(resourcesManager) {
    setProperty(tipProperty, tr("Display all recipes."));
}

int RecipesWrapper::childrenCount() const {
    return myResourcesManager->recipeMaxIngredients();
}

AssetWrapper *RecipesWrapper::child(int index) {
    return myResourcesManager->wrap(get_formulalist(index + 1), this);
}

int RecipesWrapper::childIndex(AssetWrapper *child) {
    int index = 0;
    while (index < myResourcesManager->recipeMaxIngredients()) {
        if (myResourcesManager->wrap(get_formulalist(index + 1), this) == child) {
            return index;
        }
        index++;
    }
    return -1;
}

AssetWrapper::PossibleUse RecipesWrapper::uses(const AssetWrapper *asset, std::string &) const {
    return dynamic_cast<const ArchetypeWrapper *>(asset) ? ChildrenMayUse : DoesntUse;
}
