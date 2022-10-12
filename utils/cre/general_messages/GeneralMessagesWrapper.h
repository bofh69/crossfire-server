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

#ifndef GENERAL_MESSAGES_WRAPPER_H
#define GENERAL_MESSAGES_WRAPPER_H

#include "assets.h"
#include "book.h"
#include "AssetsManager.h"
#include "assets/AssetsCollectionWrapper.h"
#include "faces/FaceWrapper.h"
#include "quests/QuestWrapper.h"

class ResourcesManager;

class GeneralMessagesWrapper : public AssetsCollectionWrapper<GeneralMessage> {
    Q_OBJECT

public:
    GeneralMessagesWrapper(AssetWrapper *parent, ResourcesManager *resources)
        : AssetsCollectionWrapper(parent, tr("General messages"), getManager()->messages(), resources, tr("Display all general messages.")) {
    }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        return dynamic_cast<const FaceWrapper *>(asset)
                || dynamic_cast<const QuestWrapper *>(asset)
                ? ChildrenMayUse : DoesntUse;
    }
};

#endif // GENERAL_MESSAGES_WRAPPER_H
