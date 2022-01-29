#include "TreasureListWrapper.h"
#include "../ResourcesManager.h"
#include "../CREPixmap.h"
#include "MimeUtils.h"
#include "TreasureLoader.h"
#include "assets.h"
#include "AssetsManager.h"

TreasureListWrapper::TreasureListWrapper(AssetWrapper *parent, treasurelist *list, ResourcesManager *resources)
   : AssetTWrapper(parent, "TreasureList", list), myResources(resources)
{
}

int TreasureListWrapper::itemCount() const {
    int count = 0;
    auto item = myWrappedItem->items;
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
    auto item = myWrappedItem->items;
    while (item && child > 0) {
        item = item->next;
        child--;
    }
    return myResources->wrap(item, this);
}

int TreasureListWrapper::childIndex(AssetWrapper *child) {
    auto item = myWrappedItem->items;
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
        myWrappedItem->total_chance = 0;
    }
    markModified(AssetUpdated, 1);
}

void TreasureListWrapper::fixTotalChance() {
    myWrappedItem->total_chance = 0;
    auto item = myWrappedItem->items;
    while (item) {
        myWrappedItem->total_chance += item->chance;
        item = item->next;
    }
}

void TreasureListWrapper::wasModified(AssetWrapper *asset, ChangeType type, int extra) {
    myResources->treasureModified(myWrappedItem);
    if (childIndex(asset) != -1 && type == AssetUpdated) {
        if (myWrappedItem->total_chance) {
            fixTotalChance();
            markModified(AssetUpdated, 1);
            return;
        }
    }
    AssetWrapper::wasModified(asset, type, extra);
}

void TreasureListWrapper::drag(QMimeData *data) const {
    QByteArray ba(data->data(MimeUtils::TreasureList));
    QDataStream df(&ba, QIODevice::WriteOnly);
    df << QString(myWrappedItem->name);
    data->setData(MimeUtils::TreasureList, ba);
}

bool TreasureListWrapper::canDrop(const QMimeData *data, int) const {
    return
            data->hasFormat(MimeUtils::Archetype)
            || data->hasFormat(MimeUtils::TreasureList)
        ;
}

void TreasureListWrapper::drop(const QMimeData *data, int row) {
    bool modified = false;
    if (row == -1) {
        row = childrenCount();
    }

    auto archs = MimeUtils::extract(data, MimeUtils::Archetype, getManager()->archetypes());
    modified |= !archs.empty();
    for (auto arch : archs) {
        markModified(BeforeChildAdd, row);
        auto item = treasure_insert(myWrappedItem, row);
        item->item = arch;
        markModified(AfterChildAdd, row);
        row++;
    }

    auto lists = MimeUtils::extract(data, MimeUtils::TreasureList, getManager()->treasures());
    modified |= !lists.empty();
    for (auto list : lists) {
        markModified(BeforeChildAdd, row);
        auto item = treasure_insert(myWrappedItem, row);
        item->name = add_string(list->name);
        markModified(AfterChildAdd, row);
        row++;
    }
    if (modified && myWrappedItem->total_chance) {
        fixTotalChance();
        markModified(AssetUpdated, 1);
    }
}

void TreasureListWrapper::removeChild(AssetWrapper *child) {
    int index = childIndex(child);
    if (index != -1) {
        auto t = dynamic_cast<TreasureWrapper *>(child);
        markModified(BeforeChildRemove, index);
        myResources->remove(t->wrappedItem());
        treasure_remove_item(myWrappedItem, index);
        markModified(AfterChildRemove, index);
        if (myWrappedItem->total_chance) {
            fixTotalChance();
            markModified(AssetUpdated, 1);
            return;
        }
    }
}
