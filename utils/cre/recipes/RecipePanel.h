#ifndef RECIPE_PANEL_H
#define RECIPE_PANEL_H

#include <QObject>
#include <QtWidgets>

extern "C" {
#include "global.h"
#include "recipe.h"
}

#include "assets/AssetWrapperPanel.h"

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
