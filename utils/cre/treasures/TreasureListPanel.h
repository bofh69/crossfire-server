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
