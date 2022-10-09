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

#ifndef ARTIFACT_LIST_PANEL_H
#define ARTIFACT_LIST_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"

#include "global.h"
#include "artifact.h"

class ArtifactListPanel : public AssetTWrapperPanel<const artifactlist> {
Q_OBJECT

public:
    ArtifactListPanel(QWidget* parent);
    virtual void updateItem() override;
};

#endif // ARTIFACT_LIST_PANEL_H
