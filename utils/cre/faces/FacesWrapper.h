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

#ifndef FACES_WRAPPER_H
#define FACES_WRAPPER_H

#include "assets.h"
#include "AssetsManager.h"
#include "assets/AssetsCollectionWrapper.h"

class FacesWrapper : public AssetsCollectionWrapper<Face> {
public:
    FacesWrapper(AssetWrapper *parent, ResourcesManager *resources) : AssetsCollectionWrapper<Face>(parent, "Faces", getManager()->faces(), resources, tr("Display all faces.")) {
    }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        if (dynamic_cast<const FaceWrapper *>(asset)) {
            return ChildrenMayUse;
        }
        return DoesntUse;
    }
};

#endif /* FACES_WRAPPER_H */
