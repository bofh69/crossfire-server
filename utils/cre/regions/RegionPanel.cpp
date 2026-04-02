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

#include "RegionPanel.h"

RegionPanel::RegionPanel(QWidget* parent) : AssetWrapperPanel(parent) {
    addLabel(tr("Short name:"), "shortName");
    addLabel(tr("Long name:"), "longName");
    addLabel(tr("Message:"), "message")->setWordWrap(true);
    addLabel(tr("Jail:"), "jailPath");
    addLabel("", "jailX");
    addLabel("", "jailY");
    addBottomFiller();
}
