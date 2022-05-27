#ifndef TREASURE_LIST_PANEL_H
#define TREASURE_LIST_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"

class TreasureListWrapper;
class AssetModel;

class CRETreasurePanel : public AssetWrapperPanel {
    Q_OBJECT

public:
    CRETreasurePanel(AssetModel* model, QWidget* parent);

    virtual void setItem(AssetWrapper *asset) override;

public slots:
    void onGenerate(bool pressed);

protected:
    TreasureListWrapper *myTreasure;
    QTreeWidget* myGenerated;
    QSpinBox* myDifficulty;

};

#endif // TREASURE_LIST_PANEL_H
