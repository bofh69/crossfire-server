#include "TreasureWrapper.h"
#include "../ResourcesManager.h"
#include "assets.h"
#include "AssetsManager.h"

TreasureWrapper::TreasureWrapper(AssetWrapper *parent, treasure *tr, ResourcesManager *resources)
   : AssetTWrapper(parent, "Treasure", tr), myResources(resources), myNextYes(nullptr), myNextNo(nullptr)
{
    if (myItem->next_yes) {
        myNextYes = new TreasureYesNo(this, myItem->next_yes, resources, TreasureYesNo::Yes);
    }
    if (myItem->next_no) {
        myNextNo = new TreasureYesNo(this, myItem->next_no, resources, TreasureYesNo::No);
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

TreasureYesNo::TreasureYesNo(TreasureWrapper *parent, treasure *tr, ResourcesManager *resources, YesNo yesNo)
    : AssetWrapper(parent, "empty"), myYesNo(yesNo) {
        myWrapped = resources->wrap(tr, this);
    }
