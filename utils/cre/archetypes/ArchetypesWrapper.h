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

#ifndef ARCHETYPES_WRAPPER_H
#define ARCHETYPES_WRAPPER_H

#include <QObject>

#include "assets.h"
#include "AssetsManager.h"
#include "global.h"
#include "assets/AssetsCollectionWrapper.h"

class ResourcesManager;

class ArchetypesWrapper : public AssetsCollectionWrapper<archetype> {
    Q_OBJECT

public:
    ArchetypesWrapper(AssetWrapper *parent, ResourcesManager *resources) : AssetsCollectionWrapper(parent, tr("Archetypes"), getManager()->archetypes(), resources, tr("Display all archetypes.")) {
        myResources = resources;
    }

    virtual PossibleUse uses(const AssetWrapper *, std::string &) const override {
        return ChildrenMayUse;
    }

    virtual void fillMenu(QMenu *menu) override;

protected:
    void addArchetype();

    ResourcesManager *myResources;
};

#endif // ARCHETYPE_WRAPPER_H
