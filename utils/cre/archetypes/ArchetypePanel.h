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

#ifndef ARCHETYPE_PANEL_H
#define ARCHETYPE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "../assets/AssetWrapperPanel.h"

#include "global.h"

class AssetModel;

/**
 * Display and allow edition of archetypes.
 */
class ArchetypePanel : public AssetWrapperPanel {
    Q_OBJECT

public:
    ArchetypePanel(AssetModel* model, QWidget* parent);
};

#endif // ARCHETYPE_PANEL_H
