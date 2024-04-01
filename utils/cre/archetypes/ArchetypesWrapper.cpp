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

#include "assets/AssetOriginAndCreationDialog.h"

void ArchetypesWrapper::fillMenu(QMenu *menu) {
    connect(menu->addAction(tr("Add archetype")), &QAction::triggered, [this] () { addArchetype(); });
}

void ArchetypesWrapper::addArchetype() {
    auto origins = myResources->archetypeFiles();
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
    myResources->assetDefined(arch, aqd.file().toStdString());
    myResources->archetypeModified(arch);
    markModified(BeforeChildAdd, myAssets.size());
    myAssets.push_back(myResources->wrap(arch, this));
    markModified(AfterChildAdd, myAssets.size());
}
