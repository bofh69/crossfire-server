#include "TreasureWrapper.h"
#include "../ResourcesManager.h"
#include "assets.h"
#include "AssetsManager.h"

TreasureWrapper::TreasureWrapper(AssetWrapper *parent, treasure *tr, ResourcesManager *resources)
   : AssetTWrapper(parent, "TreasureList", tr), myResources(resources), myNextYes(nullptr), myNextNo(nullptr)
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
    displayParent()->displayFillPanel(panel);
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

TreasureYesNo::TreasureYesNo(TreasureWrapper *parent, treasure *tr, ResourcesManager *resources, YesNo yesNo)
    : AssetWrapper(parent, "TreasureList"), myYesNo(yesNo) {
        myWrapped = resources->wrap(tr, this);
    }
