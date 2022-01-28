#include "ArchetypeWrapper.h"
#include <QMimeData>
#include "../ResourcesManager.h"
#include "../CREPixmap.h"
#include "faces/FaceWrapper.h"
#include "assets.h"
#include "AssetsManager.h"
#include "animations/AnimationWrapper.h"
#include "MimeUtils.h"

ArchetypeWrapper::ArchetypeWrapper(AssetWrapper *parent, archetype *arch, ResourcesManager *resourcesManager)
 : AssetWithArtifacts<archetype>(parent, "Archetype", arch, resourcesManager) {
    if (appearsOnTreasureList()) {
        setSpecificItem(&arch->clone, false);
    }
}

QString ArchetypeWrapper::name() const {
    return myItem->name;
}

QObject* ArchetypeWrapper::clone() {
    return myResources->wrap(&myItem->clone, this);
}

QObject *ArchetypeWrapper::head() const {
    return myItem->head ? myResources->wrap(myItem->head, nullptr) : nullptr;
}

QObject *ArchetypeWrapper::more() const {
    return myItem->more ? myResources->wrap(myItem->more, nullptr) : nullptr;
}

int ArchetypeWrapper::childrenCount() const {
    if (myItem->head) {
        return 0;
    }
    int count = 0;
    auto part = myItem;
    while (part->more) {
        count++;
        part = part->more;
    }
    return count + myArtifacts.size();
}

AssetWrapper *ArchetypeWrapper::child(int index) {
    auto part = myItem->more;
    while (index > 0 && part) {
        part = part->more;
        index--;
    }
    if (part) {
        return myResources->wrap(part, this);
    }
    return AssetWithArtifacts<archetype>::child(index);
}

int ArchetypeWrapper::childIndex(AssetWrapper *child) {
    if (myItem->head) {
        return -1;
    }
    auto part = myResources->wrap(myItem->more, this);
    int index = 0;
    while (part != child && part) {
        part = myResources->wrap(part->item()->more, this);
        index++;
    }
    if (part) {
        return index;
    }
    return AssetWithArtifacts<archetype>::childIndex(child);
}

static bool treasureContains(const treasure *t, const archetype *arch) {
    while (t) {
        if (t->item == arch)
            return true;
        if (t->name) {
            auto other = getManager()->treasures()->find(t->name);
            if (other && treasureContains(other->items, arch))
                return true;
        }
        if (t->next_yes && treasureContains(t->next_yes, arch))
            return true;
        if (t->next_no && treasureContains(t->next_no, arch))
            return true;
        t = t->next;
    }
    return false;
}

static bool isValidArchFlesh(const archetype *arch, const Face *fleshFace) {
    if (!arch->clone.randomitems || !fleshFace)
        return false;

    std::vector<const archetype *> sources;
    getManager()->archetypes()->each([&sources, &fleshFace] (const archetype * candidate) {
        if (candidate->clone.face == fleshFace)
            sources.push_back(candidate);
    });

    for (auto source : sources) {
        if (treasureContains(arch->clone.randomitems->items, source))
            return true;
    }

    return false;
}

AssetWrapper::PossibleUse ArchetypeWrapper::uses(const AssetWrapper *asset, std::string &hint) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face != nullptr) {
        auto key = object_get_value(&myItem->clone, "identified_face");
        if (face->item() == myItem->clone.face || (key && strcmp(face->item()->name, key) == 0)) {
            return Uses;
        }

        auto len = strlen(myItem->name);
        if (strncmp(myItem->name, face->item()->name, len) == 0 && face->item()->name[len] == '_') {
            auto flesh = getManager()->faces()->find(face->item()->name + len + 1);
            if (isValidArchFlesh(myItem, flesh)) {
                hint = "flesh face";
                return Uses;
            }
        }

        return ChildrenMayUse;
    }
    auto anim = dynamic_cast<const AnimationWrapper *>(asset);
    if (anim) {
        auto key = object_get_value(&myItem->clone, "identified_animation");
        if (anim->item() == myItem->clone.animation || (key && strcmp(anim->item()->name, key) == 0)) {
            return Uses;
        }
        return DoesntUse;
    }
    return DoesntUse;
}

void ArchetypeWrapper::drag(QMimeData *data) const {
    MimeUtils::addMime(data, MimeUtils::Archetype, myItem->name);
}

bool ArchetypeWrapper::appearsOnTreasureList() const {
    std::function<bool(const treasure *item)> ci = [&] (const treasure *item) {
        if (item->item == myItem) {
            return true;
        }
        if (item->next_yes && ci(item->next_yes)) {
            return true;
        }
        if (item->next_no && ci(item->next_no)) {
            return true;
        }
        if (item->next && ci(item->next)) {
            return true;
        }
        return false;
    };
    auto cl = [&] (const treasurelist *item) { return ci(item->items); };
    return getManager()->treasures()->first(cl) != nullptr;
}
