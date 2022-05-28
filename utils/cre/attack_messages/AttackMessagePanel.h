#ifndef ATTACK_MESSAGE_PANEL_H
#define ATTACK_MESSAGE_PANEL_H

#include <QWidget>
#include "../assets/AssetWrapperPanel.h"
#include "AttackMessagesWrapper.h"

class QTableWidget;
class QLabel;

class AttackMessagePanel : public AssetSWrapperPanel<SingleAttackWrapper> {
    Q_OBJECT
public:
    AttackMessagePanel(QWidget* parent);

    virtual void updateItem() override;

private slots:
    void currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    uint8_t myAttackType;
    QTableWidget *myMessages;
    QLabel *myForHitter;
    QLabel *myForVictim;
};

#endif /* ATTACK_MESSAGE_PANEL_H */
