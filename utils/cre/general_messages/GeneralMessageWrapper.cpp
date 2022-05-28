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

#include "GeneralMessageWrapper.h"
#include "ResourcesManager.h"

AssetWrapper::PossibleUse GeneralMessageWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *> (asset);
    if (face) {
        return myWrappedItem->face == face->wrappedItem() ? Uses : DoesntUse;
    }
    auto quest = dynamic_cast<const QuestWrapper *>(asset);
    if (quest && myWrappedItem->quest_code) {
        std::string name(myWrappedItem->quest_code);
        size_t len = strlen(quest->wrappedItem()->quest_code);
        return (name.length() > len && name[len] == ' ' && name.substr(0, len) == quest->wrappedItem()->quest_code) ? Uses : DoesntUse;
    }
    return DoesntUse;
}

void GeneralMessageWrapper::wasModified(AssetWrapper *asset, ChangeType type, int extra) {
    myResources->generalMessageModified(myWrappedItem);
    AssetWrapper::wasModified(asset, type, extra);
}

void GeneralMessageWrapper::setTitle(const QString &title) {
    if (myWrappedItem->title != title) {
        FREE_AND_COPY_IF(myWrappedItem->title, title.toLocal8Bit().data());
        markModified(AssetUpdated);
    }
}

void GeneralMessageWrapper::setQuest(const QString &quest) {
    if (myWrappedItem->quest_code != quest) {
        FREE_AND_COPY_IF(myWrappedItem->quest_code, quest.toLocal8Bit().data());
        markModified(AssetUpdated);
    }
}

void GeneralMessageWrapper::setChance(int chance) {
    if (chance != myWrappedItem->chance) {
        myWrappedItem->chance = chance;
        markModified(AssetUpdated);
    }
}

void GeneralMessageWrapper::setFace(const Face *face) {
    if (myWrappedItem->face != face) {
        myWrappedItem->face = face;
        markModified(AssetUpdated);
    }
}

void GeneralMessageWrapper::setMessage(const QString &message) {
    auto msg(message);
    if (!msg.isEmpty() && msg[msg.size() - 1] != '\n') {
        msg += "\n";
    }
    if (myWrappedItem->message != msg) {
        FREE_AND_COPY_IF(myWrappedItem->message, msg.toLocal8Bit().data());
        markModified(AssetUpdated);
    }
}
