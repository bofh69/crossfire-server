#include "RecipesWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "artifacts/ArtifactsWrapper.h"

RecipesWrapper::RecipesWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager)
 : AssetWrapper(parent), myResourcesManager(resourcesManager) {
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
