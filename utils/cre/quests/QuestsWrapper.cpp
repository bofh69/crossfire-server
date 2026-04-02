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

#include <QtWidgets>

#include "QuestsWrapper.h"
#include "AssetsManager.h"
#include "assets/AssetOriginAndCreationDialog.h"

void QuestsWrapper::fillMenu(QMenu *menu) {
    connect(menu->addAction(tr("Add quest")), &QAction::triggered, [this] () { addQuest(); });
}

void QuestsWrapper::addQuest() {
    auto origins = myResources->questFiles();
    auto keys = getManager()->quests()->keys();

    AssetOriginAndCreationDialog aqd(AssetOriginAndCreationDialog::Quest, AssetOriginAndCreationDialog::CreateAsset, "", origins, keys);
    if (aqd.exec() != QDialog::Accepted) {
        return;
    }

    quest_definition *qd = quest_create(aqd.code().toStdString().c_str());
    qd->quest_title = add_string(qd->quest_code);
    getManager()->quests()->define(qd->quest_code, qd);
    myResources->assetDefined(qd, aqd.file().toStdString());
    myResources->questModified(qd);
    markModified(BeforeChildAdd, myAssets.size());
    myAssets.push_back(myResources->wrap(qd, this));
    markModified(AfterChildAdd, myAssets.size());
}
