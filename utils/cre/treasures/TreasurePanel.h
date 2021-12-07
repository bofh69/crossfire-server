#ifndef TREASURE_PANEL_H
#define TREASURE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"

class ArchetypeComboBox;
class TreasureListComboBox;

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
