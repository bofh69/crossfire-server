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

#ifndef FACE_WRAPPER_H
#define FACE_WRAPPER_H

#include <QObject>

#include "global.h"
#include "assets/AssetWrapper.h"
#include "CREPixmap.h"

class ResourcesManager;

class FaceWrapper : public AssetTWrapper<Face> {
    Q_OBJECT

    Q_PROPERTY(QString name READ name)

public:
    FaceWrapper(AssetWrapper *parent, Face* face, ResourcesManager *resourcesManager);

    virtual QString displayName() const override { return myWrappedItem->name; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myWrappedItem); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    QString name() const { return myWrappedItem->name; }

protected:
    ResourcesManager *myResourcesManager;
};

#endif // FACE_WRAPPER_H
