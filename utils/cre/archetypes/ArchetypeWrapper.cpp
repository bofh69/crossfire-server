/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "ArchetypeWrapper.h"
#include <QMimeData>
#include "../ResourcesManager.h"
#include "../CREPixmap.h"
#include "faces/FaceWrapper.h"
#include "assets.h"
#include "ArchetypeLoader.h"
#include "AssetsManager.h"
#include "animations/AnimationWrapper.h"
#include "treasures/TreasureListWrapper.h"
#include "MimeUtils.h"
#include "assets/AssetWrapperPanel.h"
#include "sounds/SoundFile.h"

ArchetypeWrapper::ArchetypeWrapper(AssetWrapper *parent, archetype *arch, ResourcesManager *resourcesManager)
 : AssetWithArtifacts<archetype>(parent, "Archetype", arch, resourcesManager) {
    if (appearsOnTreasureList()) {
        setSpecificItem(&arch->clone, false);
    }

    StringBuffer* dump = stringbuffer_new();
    object_dump(&myWrappedItem->clone, dump);
    char* final = stringbuffer_finish(dump);
    myRaw = final;
    free(final);

    // Hacks: replace initial "arch" by "Object", and remove the "more 0" line which messes things
    myRaw.replace(0, 4, "Object");
    auto more = myRaw.find("more 0\n");
    if (more != std::string::npos) {
        myRaw.erase(more, 7);
    }
}

void ArchetypeWrapper::wasModified(AssetWrapper *asset, ChangeType type, int extra) {
    myResources->archetypeModified(myWrappedItem);
    AssetTWrapper::wasModified(asset, type, extra);
}

QString ArchetypeWrapper::name() const {
    return myWrappedItem->name;
}

QObject* ArchetypeWrapper::clone() {
    return myResources->wrap(&myWrappedItem->clone, this);
}

QObject *ArchetypeWrapper::head() const {
    return myWrappedItem->head ? myResources->wrap(myWrappedItem->head, nullptr) : nullptr;
}

QObject *ArchetypeWrapper::more() const {
    return myWrappedItem->more ? myResources->wrap(myWrappedItem->more, nullptr) : nullptr;
}

int ArchetypeWrapper::childrenCount() const {
    if (myWrappedItem->head) {
        return 0;
    }
    int count = 0;
    auto part = myWrappedItem;
    while (part->more) {
        count++;
        part = part->more;
    }
    return count + myArtifacts.size();
}

AssetWrapper *ArchetypeWrapper::child(int index) {
    auto part = myWrappedItem->more;
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
    if (myWrappedItem->head) {
        return -1;
    }
    int index = 0;
    if (myWrappedItem->more) {
        auto part = myResources->wrap(myWrappedItem->more, this);
        while (part != child && part) {
            part = myResources->wrap(part->wrappedItem()->more, this);
            index++;
        }
        if (part) {
            return index;
        }
    }
    return index + AssetWithArtifacts<archetype>::childIndex(child);
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
        auto key = object_get_value(&myWrappedItem->clone, "identified_face");
        if (face->wrappedItem() == myWrappedItem->clone.face || (key && strcmp(face->wrappedItem()->name, key) == 0)) {
            return Uses;
        }

        auto len = strlen(myWrappedItem->name);
        if (strncmp(myWrappedItem->name, face->wrappedItem()->name, len) == 0 && face->wrappedItem()->name[len] == '_') {
            auto flesh = getManager()->faces()->find(face->wrappedItem()->name + len + 1);
            if (isValidArchFlesh(myWrappedItem, flesh)) {
                hint = "flesh face";
                return Uses;
            }
        }

        return ChildrenMayUse;
    }
    auto anim = dynamic_cast<const AnimationWrapper *>(asset);
    if (anim) {
        auto key = object_get_value(&myWrappedItem->clone, "identified_animation");
        if (anim->wrappedItem() == myWrappedItem->clone.animation || (key && strcmp(anim->wrappedItem()->name, key) == 0)) {
            return Uses;
        }
        return DoesntUse;
    }
    auto tl = dynamic_cast<const TreasureListWrapper *>(asset);
    if (tl) {
        return myWrappedItem->clone.randomitems == tl->wrappedItem() ? Uses : DoesntUse;
    }
    auto arch = dynamic_cast<const ArchetypeWrapper *>(asset);
    if (arch) {
        if (wrappedItem()->clone.other_arch == arch->wrappedItem()) {
            hint = "other arch";
            return Uses;
        }
        sstring death_anim = NULL;
        if ((death_anim = object_get_value(&myWrappedItem->clone, "death_animation")) && strcmp(death_anim, arch->wrappedItem()->name) == 0) {
            hint = "death animation";
            return Uses;
        }
    }
    auto sound = dynamic_cast<const SoundFile *>(asset);
    if (sound) {
        if (myWrappedItem->clone.type != SKILL && myWrappedItem->clone.type != SPELL && myWrappedItem->clone.type != SPELL_EFFECT) {
            return DoesntUse;
        }
        bool uses = std::any_of(sound->events().cbegin(), sound->events().cend(), [this] (const std::string &event) {
            return event == myWrappedItem->name || event == myWrappedItem->clone.name;
        });
        return uses ? Uses : DoesntUse;
    }
    return DoesntUse;
}

void ArchetypeWrapper::drag(QMimeData *data) const {
    MimeUtils::addMime(data, MimeUtils::Archetype, myWrappedItem->name);
}

bool ArchetypeWrapper::appearsOnTreasureList() const {
    std::function<bool(const treasure *item)> ci = [&] (const treasure *item) {
        if (item->item == myWrappedItem) {
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
    auto cl = [&] (const treasurelist *item) { return item->items && ci(item->items); };
    return getManager()->treasures()->first(cl) != nullptr;
}

QString ArchetypeWrapper::raw() const {
    return QString::fromStdString(myRaw);
}

void ArchetypeWrapper::setRaw(const QString &raw) {
    if (raw.toStdString() != myRaw) {
        myRaw = raw.toStdString();
        BufferReader *br = bufferreader_init_from_memory(nullptr, myRaw.data(), myRaw.length());

        auto origin = myResources->originOf(myWrappedItem);
        ArchetypeLoader loader(getManager()->archetypes(), nullptr);
        loader.load(br, origin);
        bufferreader_destroy(br);

        markModified(AssetUpdated);
    }
}
