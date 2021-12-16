#include "TreasureWrapper.h"
#include "TreasureListWrapper.h"
#include "../ResourcesManager.h"
#include "assets.h"
#include "AssetsManager.h"
#include "MimeUtils.h"

TreasureWrapper::TreasureWrapper(AssetWrapper *parent, treasure *tr, ResourcesManager *resources)
   : AssetTWrapper(parent, "Treasure", tr), myResources(resources), myNextYes(nullptr), myNextNo(nullptr)
{
    if (myItem->next_yes) {
        myNextYes = new TreasureYesNo(this, myItem->next_yes, resources, true);
    }
    if (myItem->next_no) {
        myNextNo = new TreasureYesNo(this, myItem->next_no, resources, false);
    }
}

QString TreasureWrapper::displayName() const {
    QString name;
    if (myItem->item) {
        name = myItem->item->clone.name;
    }
    else if (!myItem->name || strcmp(myItem->name, "NONE") == 0) {
        name = "Nothing";
    }
    else {
        name = myItem->name;
    }

    auto tlw = dynamic_cast<TreasureListWrapper *>(displayParent());
    if (tlw && tlw->totalChance() != 0) {
        name = tr("%1 (%2%3%, %4 chances on %5)")
            .arg(name)
            .arg(myItem->nrof > 0 ? tr("1 to %2, ").arg(myItem->nrof) : "")
            .arg(qRound((float)100 * myItem->chance / tlw->totalChance()))
            .arg(myItem->chance)
            .arg(tlw->totalChance());
    } else {
        name = tr("%1 (%2%3%)")
            .arg(name)
            .arg(myItem->nrof > 0 ? tr("1 to %2, ").arg(myItem->nrof) : "")
            .arg(myItem->chance);
    }

    return name;
}

QIcon TreasureWrapper::displayIcon() const {
    if (myItem->item) {
        return CREPixmap::getIcon(myItem->item->clone.face);
    }
    if (!myItem->name || strcmp(myItem->name, "NONE") == 0) {
        return QIcon();
    }

    auto tl = getManager()->treasures()->find(myItem->name);
    if (!tl) {
        return QIcon();
    }
    return tl->total_chance == 0 ? CREPixmap::getTreasureIcon() : CREPixmap::getTreasureOneIcon();
}

void TreasureWrapper::displayFillPanel(QWidget *panel) {
    AssetWrapperPanel *p = static_cast<AssetWrapperPanel *>(panel);
    p->setItem(this);
}

int TreasureWrapper::childrenCount() const {
    int count = 0;
    if (myItem->next_yes) {
        count++;
    }
    if (myItem->next_no) {
        count++;
    }
    return count;
}

AssetWrapper *TreasureWrapper::child(int child) {
    if (myNextYes) {
        if (child == 0) {
            return myNextYes;
        }
        child--;
    }
    if (myNextNo) {
        if (child == 0) {
            return myNextNo;
        }
    }
    return nullptr;
}

int TreasureWrapper::childIndex(AssetWrapper *child) {
    int index = 0;
    if (myNextYes) {
        if (child == myNextYes) {
            return index;
        }
        index++;
    }
    if (myNextNo) {
        if (child == myNextNo) {
            return index;
        }
    }
    return -1;
}

void TreasureWrapper::doRemoveChild(TreasureYesNo **tr, treasure **ti, int index) {
    markModified(BeforeChildRemove, index);
    myResources->remove(*ti);
    treasure_free(*ti);
    (*ti) = nullptr;
    delete *tr;
    (*tr) = nullptr;
    markModified(AfterChildRemove, index);
    return;
}

void TreasureWrapper::removeChild(AssetWrapper *child) {
    if (child == myNextYes) {
        doRemoveChild(&myNextYes, &myItem->next_yes, 0);
        return;
    }
    if (child == myNextNo) {
        doRemoveChild(&myNextNo, &myItem->next_no, myNextYes ? 1 : 0);
    }
}

void TreasureWrapper::doAddChild(TreasureYesNo **my, treasure **ti, bool isYes, int index, treasurelist *tl, archetype *arch) {
    markModified(BeforeChildAdd, index);
    (*ti) = get_empty_treasure();
    (*my) = new TreasureYesNo(this, *ti, myResources, isYes);
    (*ti)->item = arch;
    if (tl) {
        (*ti)->name = add_string(tl->name);
    }
    markModified(AfterChildAdd, index);
}

void TreasureWrapper::addChild(treasurelist *tl, archetype *arch) {
    if (!myNextYes) {
        doAddChild(&myNextYes, &myItem->next_yes, true, 0, tl, arch);
        return;
    }
    if (!myNextNo) {
        doAddChild(&myNextNo, &myItem->next_no, false, 1, tl, arch);
    }
}

bool TreasureWrapper::canDrop(const QMimeData *data, int) const {
    return
            data->hasFormat(MimeUtils::Archetype)
            || data->hasFormat(MimeUtils::TreasureList)
        ;
}

void TreasureWrapper::drop(const QMimeData *data, int) {
    auto archs = MimeUtils::extract(data, MimeUtils::Archetype, getManager()->archetypes());
    for (auto arch : archs) {
        addChild(nullptr, arch);
    }

    auto lists = MimeUtils::extract(data, MimeUtils::TreasureList, getManager()->treasures());
    for (auto list : lists) {
        addChild(list, nullptr);
    }
}

uint8_t TreasureWrapper::chance() const {
    return myItem->chance;
}

void TreasureWrapper::setChance(uint8_t chance) {
    if (chance != myItem->chance) {
        myItem->chance = chance;
        markModified(AssetUpdated);
    }
}

uint8_t TreasureWrapper::magic() const {
    return myItem->magic;
}

void TreasureWrapper::setMagic(uint8_t magic) {
    if (magic != myItem->magic) {
        myItem->magic = magic;
        markModified(AssetUpdated);
    }
}

uint16_t TreasureWrapper::nrof() const {
    return myItem->nrof;
}

void TreasureWrapper::setNrof(uint16_t nrof) {
    if (nrof != myItem->nrof) {
        myItem->nrof = nrof;
        markModified(AssetUpdated);
    }
}

const treasurelist *TreasureWrapper::list() const {
    return myItem->name ? getManager()->treasures()->find(myItem->name) : nullptr;
}

void TreasureWrapper::setList(const treasurelist *list) {
    if (myItem->name != (list ? list->name : nullptr)) {
        FREE_AND_CLEAR_STR_IF(myItem->name);
        if (list) {
            myItem->name = add_string(list->name);
            myItem->item = nullptr;
        }
        markModified(AssetUpdated);
    }
}

const archetype *TreasureWrapper::arch() const {
    return myItem->item;
}

void TreasureWrapper::setArch(const archetype *arch) {
    if (arch != myItem->item) {
        myItem->item = const_cast<archetype *>(arch);
        if (myItem->item && myItem->name) {
            FREE_AND_CLEAR_STR(myItem->name);
        }
        markModified(AssetUpdated);
    }
}

void TreasureWrapper::fillMenu(QMenu *menu) {
    connect(menu->addAction(tr("Delete")), &QAction::triggered, [this] () { myParent->removeChild(this); });
    if (myNextYes || myNextNo) {
        connect(menu->addAction(tr("Swap 'yes' and 'no'")), &QAction::triggered, this, &TreasureWrapper::swapYesNo);
    }
}

void TreasureWrapper::swapYesNo() {
    if (myNextYes || myNextNo) {
        markModified(BeforeLayoutChange);
        if (myNextYes) {
            myNextYes->setIsYes(false);
        }
        if (myNextNo) {
            myNextNo->setIsYes(true);
        }
        std::swap(myNextYes, myNextNo);
        std::swap(myItem->next_yes, myItem->next_no);
        markModified(AfterLayoutChange);
    }
}

TreasureYesNo::TreasureYesNo(TreasureWrapper *parent, treasure *tr, ResourcesManager *resources, bool isYes)
    : AssetWrapper(parent, "empty"), myIsYes(isYes) {
        myWrapped = resources->wrap(tr, this);
    }

void TreasureYesNo::fillMenu(QMenu *menu) {
    connect(menu->addAction(tr("Delete")), &QAction::triggered, [this] () { myParent->removeChild(this); });
    connect(menu->addAction(tr("Swap 'yes' and 'no'")), &QAction::triggered, static_cast<TreasureWrapper *>(myParent), &TreasureWrapper::swapYesNo);
}