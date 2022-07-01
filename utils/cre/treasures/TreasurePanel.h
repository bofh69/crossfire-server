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

#ifndef TREASURE_PANEL_H
#define TREASURE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"

class ArchetypeComboBox;
class TreasureListComboBox;

/**
 * Display and allow edition of a treasure in a treasure list.
 */
class TreasurePanel : public AssetWrapperPanel {
    Q_OBJECT

public:
    TreasurePanel(QWidget *parent);

protected:
    ArchetypeComboBox *myArch;
    TreasureListComboBox *myList;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
};

#endif // TREASURE_PANEL_H
