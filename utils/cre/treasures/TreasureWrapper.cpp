#include "TreasureWrapper.h"
#include "TreasureListWrapper.h"
#include "../ResourcesManager.h"
#include "assets.h"
#include "AssetsManager.h"
#include "MimeUtils.h"
#include "archetypes/ArchetypeWrapper.h"

extern "C" {
#include "global.h"
#include "artifact.h"
#include "libproto.h"
}

TreasureWrapper::TreasureWrapper(AssetWrapper *parent, treasure *tr, ResourcesManager *resources)
   : AssetWithArtifacts<treasure>(parent, "Treasure", tr, resources), myNextYes(nullptr), myNextNo(nullptr)
{
    if (myWrappedItem->next_yes) {
        myNextYes = new TreasureYesNo(this, myWrappedItem->next_yes, resources, true);
    }
    if (myWrappedItem->next_no) {
        myNextNo = new TreasureYesNo(this, myWrappedItem->next_no, resources, false);
    }
    if (tr->item) {
        setSpecificItem(&tr->item->clone, false);
    }
}

TreasureWrapper::~TreasureWrapper() {
}

QString TreasureWrapper::displayName() const {
    QString name;
    if (myWrappedItem->item) {
        name = tr("%1 (%2)").arg(myWrappedItem->item->clone.name, myWrappedItem->item->name);
    }
    else if (!myWrappedItem->name || strcmp(myWrappedItem->name, "NONE") == 0) {
        name = "Nothing";
    }
    else {
        name = myWrappedItem->name;
    }

    auto tlw = dynamic_cast<TreasureListWrapper *>(displayParent());
    if (tlw && tlw->totalChance() != 0) {
        name = tr("%1 (%2%3%, %4 chances on %5)")
            .arg(name)
            .arg(myWrappedItem->nrof > 0 ? tr("1 to %2, ").arg(myWrappedItem->nrof) : "")
            .arg(qRound((float)100 * myWrappedItem->chance / tlw->totalChance()))
            .arg(myWrappedItem->chance)
            .arg(tlw->totalChance());
    } else {
        name = tr("%1 (%2%3%)")
            .arg(name)
            .arg(myWrappedItem->nrof > 0 ? tr("1 to %2, ").arg(myWrappedItem->nrof) : "")
            .arg(myWrappedItem->chance);
    }

    return name;
}

QIcon TreasureWrapper::displayIcon() const {
    if (myWrappedItem->item) {
        return CREPixmap::getIcon(myWrappedItem->item->clone.face);
    }
    if (!myWrappedItem->name || strcmp(myWrappedItem->name, "NONE") == 0) {
        return QIcon();
    }

    auto tl = getManager()->treasures()->find(myWrappedItem->name);
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
    if (myWrappedItem->next_yes) {
        count++;
    }
    if (myWrappedItem->next_no) {
        count++;
    }
    return count + AssetWithArtifacts<treasure>::childrenCount();
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
        child--;
    }
    return AssetWithArtifacts<treasure>::child(child);
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
        index++;
    }

    auto c = AssetWithArtifacts<treasure>::childIndex(child);
    if (c != -1) {
        return c + index;
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
        doRemoveChild(&myNextYes, &myWrappedItem->next_yes, 0);
        return;
    }
    if (child == myNextNo) {
        doRemoveChild(&myNextNo, &myWrappedItem->next_no, myNextYes ? 1 : 0);
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
        doAddChild(&myNextYes, &myWrappedItem->next_yes, true, 0, tl, arch);
        return;
    }
    if (!myNextNo) {
        doAddChild(&myNextNo, &myWrappedItem->next_no, false, 1, tl, arch);
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
    return myWrappedItem->chance;
}

void TreasureWrapper::setChance(uint8_t chance) {
    if (chance != myWrappedItem->chance) {
        myWrappedItem->chance = chance;
        markModified(AssetUpdated);
    }
}

uint8_t TreasureWrapper::magic() const {
    return myWrappedItem->magic;
}

void TreasureWrapper::setMagic(uint8_t magic) {
    if (magic != myWrappedItem->magic) {
        myWrappedItem->magic = magic;
        markModified(AssetUpdated);
    }
}

uint16_t TreasureWrapper::nrof() const {
    return myWrappedItem->nrof;
}

void TreasureWrapper::setNrof(uint16_t nrof) {
    if (nrof != myWrappedItem->nrof) {
        myWrappedItem->nrof = nrof;
        markModified(AssetUpdated);
    }
}

const treasurelist *TreasureWrapper::list() const {
    return myWrappedItem->name ? getManager()->treasures()->find(myWrappedItem->name) : nullptr;
}

void TreasureWrapper::setList(const treasurelist *list) {
    if (myWrappedItem->name != (list ? list->name : nullptr)) {
        FREE_AND_CLEAR_STR_IF(myWrappedItem->name);
        if (list) {
            myWrappedItem->name = add_string(list->name);
            myWrappedItem->item = nullptr;
            setSpecificItem(nullptr, true);
        }
        markModified(AssetUpdated);
    }
}

const archetype *TreasureWrapper::arch() const {
    return myWrappedItem->item;
}

void TreasureWrapper::setArch(const archetype *arch) {
    if (arch != myWrappedItem->item) {
        myWrappedItem->item = const_cast<archetype *>(arch);
        if (myWrappedItem->item && myWrappedItem->name) {
            FREE_AND_CLEAR_STR(myWrappedItem->name);
        }
        markModified(AssetUpdated);
        setSpecificItem(arch ? &arch->clone : nullptr, true);
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
        std::swap(myWrappedItem->next_yes, myWrappedItem->next_no);
        markModified(AfterLayoutChange);
    }
}

AssetWrapper::PossibleUse TreasureWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto arch = dynamic_cast<const ArchetypeWrapper *>(asset);
    if (arch) {
        if (wrappedItem()->item == arch->wrappedItem()) {
            return Uses;
        }
        return ChildrenMayUse;
    }
    auto list = dynamic_cast<const TreasureListWrapper *>(asset);
    if (list) {
        return myWrappedItem->name == list->wrappedItem()->name ? Uses : DoesntUse;
    }
    return DoesntUse;
}

TreasureYesNo::TreasureYesNo(TreasureWrapper *parent, treasure *tr, ResourcesManager *resources, bool isYes)
    : AssetWrapper(parent, "empty"), myIsYes(isYes) {
        myWrapped = resources->wrap(tr, this);
    }

void TreasureYesNo::fillMenu(QMenu *menu) {
    connect(menu->addAction(tr("Delete")), &QAction::triggered, [this] () { myParent->removeChild(this); });
    connect(menu->addAction(tr("Swap 'yes' and 'no'")), &QAction::triggered, static_cast<TreasureWrapper *>(myParent), &TreasureWrapper::swapYesNo);
}
