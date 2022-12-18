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

#include "ObjectWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"

ObjectWrapper::ObjectWrapper(AssetWrapper *parent, object *ob, ResourcesManager *resourcesManager) : AssetTWrapper(parent, "empty", ob) {
    myResourcesManager = resourcesManager;
}

ObjectWrapper::~ObjectWrapper() {
}

AssetWrapper* ObjectWrapper::arch()
{
    return myResourcesManager->wrap(myWrappedItem->arch, this);
}

double scale_level(double l) {
    if (l < 1) {
        l = 1;
    }
    return 10*log(l)+1;
}

int ObjectWrapper::suggested_level() const {
    return scale_level(hp()*10/100 + regen()*20/100 + dps()*20/100);
}

float ObjectWrapper::dps() const {
    return damage() * FABS(speed()) / MOVE_PER_SECOND;
}

float ObjectWrapper::regen() const {
    return con() / 4.0 / MOVE_PER_SECOND;
}

AssetWrapper *ObjectWrapper::randomItems() {
    if (!myWrappedItem->randomitems) {
        return nullptr;
    }
    return myResourcesManager->wrap(myWrappedItem->randomitems, nullptr);
}
