#include "TreasureListWrapper.h"
#include "../ResourcesManager.h"
#include "../CREPixmap.h"

TreasureListWrapper::TreasureListWrapper(AssetWrapper *parent, const treasurelist *list, ResourcesManager *resources)
   : AssetTWrapper(parent, "TreasureList", list), myResources(resources)
{
}

int TreasureListWrapper::itemCount() const {
    int count = 0;
    auto item = myItem->items;
    while (item) {
        count++;
        item = item->next;
    }
    return count;
}

int TreasureListWrapper::childrenCount() const {
    return itemCount();
}

AssetWrapper *TreasureListWrapper::child(int child) {
    auto item = myItem->items;
    while (item && child > 0) {
        item = item->next;
        child--;
    }
    return myResources->wrap(item, this);
}

int TreasureListWrapper::childIndex(AssetWrapper *child) {
    auto item = myItem->items;
    int index = 0;
    while (item) {
        if (myResources->wrap(item, this) == child) {
            return index;
        }
        item = item->next;
        index++;
    }
    return -1;
}
