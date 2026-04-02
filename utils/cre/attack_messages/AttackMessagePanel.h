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

#ifndef ATTACK_MESSAGE_PANEL_H
#define ATTACK_MESSAGE_PANEL_H

#include <QWidget>
#include "../assets/AssetWrapperPanel.h"
#include "AttackMessagesWrapper.h"

class QTableWidget;
class QLabel;

/**
 * Display attack messages for a single attack type.
 */
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
