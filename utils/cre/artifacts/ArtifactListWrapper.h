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

#ifndef ARTIFACT_LIST_WRAPPER_H
#define ARTIFACT_LIST_WRAPPER_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"
class ResourcesManager;

class ArtifactListWrapper : public AssetTWrapper<artifactlist> {
Q_OBJECT

public:
    ArtifactListWrapper(AssetWrapper *parent, artifactlist *list, ResourcesManager *resourcesManager);

    virtual QString displayName() const override {
        auto data = get_typedata(myWrappedItem->type);
        return data ? data->name : tr("type %1").arg(myWrappedItem->type);
    }

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int) override;
    virtual int childIndex(AssetWrapper *) override;

    virtual void wasModified(AssetWrapper *asset, ChangeType type, int extra) override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

protected:
    ResourcesManager *myResourcesManager;
};

#endif // ARTIFACT_LIST_WRAPPER_H
