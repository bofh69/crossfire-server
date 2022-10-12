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

#ifndef ARTIFACTS_WRAPPER_H
#define ARTIFACTS_WRAPPER_H

#include <QObject>
#include <QStringList>

#include "global.h"
#include "assets/AssetWrapper.h"

class ResourcesManager;

class ArtifactsWrapper : public AssetWrapper {
Q_OBJECT

public:
    ArtifactsWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager);

    virtual QString displayName() const override { return tr("Artifacts"); }

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int) override;
    virtual int childIndex(AssetWrapper *) override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

protected:
    ResourcesManager *myResourcesManager;
};

#endif // ARTIFACTS_WRAPPER_H
