#include "RecipeWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "RecipeListWrapper.h"

RecipeListWrapper::RecipeListWrapper(AssetWrapper *parent, const recipelist *list, ResourcesManager *resources)
  : AssetTWrapper(parent, "RecipeList", list), myResourcesManager(resources) {
}

int RecipeListWrapper::childrenCount() const {
    int count = 0;
    auto item = myItem->items;
    while (item) {
        count++;
        item = item->next;
    }
    return count;
}

AssetWrapper *RecipeListWrapper::child(int index) {
    auto item = myItem->items;
    while (index > 0) {
        index--;
        item = item->next;
    }
    return myResourcesManager->wrap(item, this);
}

int RecipeListWrapper::childIndex(AssetWrapper *child) {
    auto item = myItem->items;
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
