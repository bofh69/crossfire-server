#include "TreasureListWrapper.h"
#include "../ResourcesManager.h"
#include "../CREPixmap.h"

TreasureListWrapper::TreasureListWrapper(AssetWrapper *parent, treasurelist *list, ResourcesManager *resources)
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

void TreasureListWrapper::displayFillPanel(QWidget *panel) {
    AssetWrapperPanel *p = static_cast<AssetWrapperPanel *>(panel);
    p->setItem(this);
}

void TreasureListWrapper::setSingleItem(bool isSingle) {
    if (isSingle == isSingleItem()) {
        return;
    }
    if (isSingle) {
        fixTotalChance();
    } else {
        myItem->total_chance = 0;
    }
    markModified(true);
}

void TreasureListWrapper::fixTotalChance() {
    myItem->total_chance = 0;
    auto item = myItem->items;
    while (item) {
        myItem->total_chance += item->chance;
        item = item->next;
    }
}

void TreasureListWrapper::wasModified(AssetWrapper *asset, bool updateChildren) {
    myResources->treasureModified(myItem);
    if (childIndex(asset) != -1) {
        if (myItem->total_chance) {
            fixTotalChance();
        }
        AssetWrapper::wasModified(this, true);
    } else {
        AssetWrapper::wasModified(asset, updateChildren);
    }
}

void TreasureListWrapper::drag(QMimeData *data) const {
    QByteArray ba(data->data("x-crossfire/treasure-list"));
    QDataStream df(&ba, QIODevice::WriteOnly);
    df << QString(myItem->name);
    data->setData("x-crossfire/treasure-list", ba);
}
