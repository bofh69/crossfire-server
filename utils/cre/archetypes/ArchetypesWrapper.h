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

#include "global.h"
#include "assets/AssetWrapper.h"

class ResourcesManager;

class ArchetypesWrapper : public AssetWrapper {
    Q_OBJECT

public:
    ArchetypesWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager);

    virtual QString displayName() const override { return tr("Archetypes"); }
    virtual int childrenCount() const override { return myArch.size(); }
    virtual AssetWrapper *child(int index) override { return myArch[index]; }
    virtual int childIndex(AssetWrapper *child) override { return myArch.indexOf(child); }

    virtual PossibleUse uses(const AssetWrapper *, std::string &) const override {
        return ChildrenMayUse;
    }

protected:
    QVector<AssetWrapper *> myArch;
    ResourcesManager *myResourcesManager;
};

#endif // ARCHETYPE_WRAPPER_H
