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

#ifndef FACE_SETS_WRAPPER_H
#define FACE_SETS_WRAPPER_H

#include <QObject>

extern "C"
{
#include "global.h"
#include "image.h"
}
#include "assets/AssetWrapper.h"
#include "assets/AssetWrapperPanel.h"

class ResourcesManager;

class FacesetsWrapper : public AssetTWrapper<face_sets> {
    Q_OBJECT

    Q_PROPERTY(QString prefix READ prefix)
    Q_PROPERTY(QString fullname READ fullname)
    Q_PROPERTY(QString fallback READ fallback)
    Q_PROPERTY(QString size READ size)
    Q_PROPERTY(QString extension READ extension)

public:
    FacesetsWrapper(AssetWrapper *parent, face_sets* fs, ResourcesManager *) : AssetTWrapper(parent, "Faceset", fs) { }

    virtual QString displayName() const override { return myWrappedItem->prefix; }

    QString prefix() const { return myWrappedItem->prefix; }
    QString fullname() const { return myWrappedItem->fullname; }
    QString fallback() const { return myWrappedItem->fallback ? myWrappedItem->fallback->prefix : ""; }
    QString size() const { return myWrappedItem->size; }
    QString extension() const { return myWrappedItem->extension; }
};

#endif // FACE_SETS_WRAPPER_H
