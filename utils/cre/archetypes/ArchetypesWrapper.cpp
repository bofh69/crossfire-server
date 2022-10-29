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

#include "ArchetypesWrapper.h"
#include "../ResourcesManager.h"

#include "assets.h"
#include "AssetsManager.h"

ArchetypesWrapper::ArchetypesWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager)
 : AssetWrapper(parent), myResourcesManager(resourcesManager) {
    getManager()->archetypes()->each([&] (archetype *arch) {
        if (!arch->head) {
            myArch.append(myResourcesManager->wrap(arch, this));
        }
    });
    std::sort(myArch.begin(), myArch.end(), compareByDisplayName);
    setProperty(tipProperty, tr("Display all archetypes."));
}
