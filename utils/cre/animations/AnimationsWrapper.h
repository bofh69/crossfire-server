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

#ifndef ANIMATIONS_WRAPPER_H
#define ANIMATIONS_WRAPPER_H

#include <QObject>

extern "C"
{
#include "global.h"
#include "face.h"
}
#include "assets.h"
#include "AssetsManager.h"
#include "../assets/AssetsCollectionWrapper.h"
#include "../faces/FaceWrapper.h"

class ResourcesManager;

class AnimationsWrapper : public AssetsCollectionWrapper<Animations> {
    Q_OBJECT

public:
    AnimationsWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager)
        : AssetsCollectionWrapper(parent, "Animations", getManager()->animations(), resourcesManager, tr("Display all animations.")) {
        }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        return dynamic_cast<const FaceWrapper *>(asset) == nullptr ? DoesntUse : ChildrenMayUse;
    }
};

#endif // ANIMATIONS_WRAPPER_H
