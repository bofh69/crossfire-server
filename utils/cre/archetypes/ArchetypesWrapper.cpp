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
#include "assets/AssetOriginAndCreationDialog.h"

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

void ArchetypesWrapper::fillMenu(QMenu *menu) {
    connect(menu->addAction(tr("Add archetype")), &QAction::triggered, [this] () { addArchetype(); });
}

void ArchetypesWrapper::addArchetype() {
    auto origins = myResourcesManager->archetypeFiles();
    auto keys = getManager()->archetypes()->keys();

    AssetOriginAndCreationDialog aqd(AssetOriginAndCreationDialog::Archetype, AssetOriginAndCreationDialog::CreateAsset, "", origins, keys);
    if (aqd.exec() != QDialog::Accepted) {
        return;
    }

    auto arch = get_archetype_struct();
    CLEAR_FLAG(&arch->clone, FLAG_REMOVED);
    arch->name = add_string(aqd.code().toStdString().c_str());
    arch->clone.name = add_string(arch->name);
    getManager()->archetypes()->define(arch->name, arch);
    myResourcesManager->assetDefined(arch, aqd.file().toStdString());
    myResourcesManager->archetypeModified(arch);
    markModified(BeforeChildAdd, myArch.size());
    myArch.push_back(myResourcesManager->wrap(arch, this));
    std::sort(myArch.begin(), myArch.end(), compareByDisplayName);
    markModified(AfterChildAdd, myArch.size());
}
