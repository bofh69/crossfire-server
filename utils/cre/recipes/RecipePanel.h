#ifndef RECIPE_PANEL_H
#define RECIPE_PANEL_H

#include <QObject>
#include <QtWidgets>

#include "assets/AssetWrapperPanel.h"

class RecipePanel : public AssetWrapperPanel {
    Q_OBJECT

public:
    RecipePanel(QWidget *parent);

    virtual void setItem(AssetWrapper *asset) override;

protected:
    QTreeWidget* myArchetypes;
    QTextEdit* myIngredients;
};

#endif // RECIPE_PANEL_H
