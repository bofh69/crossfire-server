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

#ifndef QUEST_WRAPPER_H
#define QUEST_WRAPPER_H

#include <QObject>

extern "C" {
#include "global.h"
#include "quest.h"
}
#include "assets/AssetWrapper.h"
#include "CREPixmap.h"

class ResourcesManager;

class QuestWrapper : public AssetTWrapper<quest_definition> {
    Q_OBJECT

public:
    QuestWrapper(AssetWrapper *parent, quest_definition* quest, ResourcesManager *) : AssetTWrapper(parent, "Quest", quest) { }

    virtual QString displayName() const override { return myWrappedItem->quest_title; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myWrappedItem->face); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;
};

#endif // QUEST_WRAPPER_H
