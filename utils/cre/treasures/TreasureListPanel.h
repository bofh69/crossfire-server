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

#ifndef TREASURE_LIST_PANEL_H
#define TREASURE_LIST_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"
extern "C" {
#include "global.h"
#include "treasure.h"
}

class TreasureListWrapper;
class AssetModel;

/**
 * Display and allow edition of a treasure list.
 */
class CRETreasurePanel : public AssetTWrapperPanel<treasurelist> {
    Q_OBJECT

public:
    CRETreasurePanel(AssetModel* model, QWidget* parent);

    virtual void updateItem() override;

public slots:
    void onGenerate(bool pressed);

protected:
    QTreeWidget* myGenerated;
    QSpinBox* myDifficulty;

};

#endif // TREASURE_LIST_PANEL_H
