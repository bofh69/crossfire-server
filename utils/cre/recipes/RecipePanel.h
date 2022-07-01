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

#ifndef RECIPE_PANEL_H
#define RECIPE_PANEL_H

#include <QObject>
#include <QtWidgets>

extern "C" {
#include "global.h"
#include "recipe.h"
}

#include "assets/AssetWrapperPanel.h"

/**
 * Display details of a crafting recipe.
 */
class RecipePanel : public AssetTWrapperPanel<recipe> {
    Q_OBJECT

public:
    RecipePanel(QWidget *parent);

    virtual void updateItem() override;

protected:
    QTreeWidget* myArchetypes;
    QTextEdit* myIngredients;
};

#endif // RECIPE_PANEL_H
