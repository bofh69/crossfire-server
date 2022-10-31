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

#include "SoundFilePanel.h"

SoundFilePanel::SoundFilePanel(QWidget *parent, AssetModel *model) : AssetSWrapperPanel(parent) {
    addLabel(tr("Filename:"), "displayName");
    addAssetUseTree(tr("Used by"), model, "self");
    addWidget(tr("License information"), myLicense = new QTreeWidget(this), false, nullptr, nullptr);
    myLicense->setColumnCount(2);
    myLicense->setHeaderLabels(QStringList(tr("License field")) << tr("Value"));
}

void SoundFilePanel::updateItem() {
    myLicense->clear();
    for (auto license : myItem->license()) {
        new QTreeWidgetItem(myLicense, QStringList(license.first.c_str()) << license.second.c_str());
    }
}
