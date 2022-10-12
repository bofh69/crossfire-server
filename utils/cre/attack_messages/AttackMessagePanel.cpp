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

#include <QTableWidget>
#include "AttackMessagePanel.h"
#include "AttackMessagesWrapper.h"
#include "global.h"
#include "attack.h"
#include "sproto.h"

AttackMessagePanel::AttackMessagePanel(QWidget* parent) : AssetSWrapperPanel(parent), myAttackType(0) {
    myMessages = addWidget(nullptr, new QTableWidget(0, 4, this), false, nullptr, nullptr);
    myMessages->setHorizontalHeaderLabels({tr("Max damage"), tr("First part"), tr("Second part"), tr("For victim")});
    myMessages->setEditTriggers(QAbstractItemView::NoEditTriggers);
    myMessages->setSelectionBehavior(QAbstractItemView::SelectRows);
    myMessages->verticalHeader()->setVisible(false);
    connect(myMessages, &QTableWidget::currentCellChanged, this, &AttackMessagePanel::currentCellChanged);
    myForHitter = addLabel(nullptr, nullptr);
    myForHitter->setTextInteractionFlags(Qt::TextSelectableByMouse);
    myForVictim = addLabel(nullptr, nullptr);
    myForVictim->setTextInteractionFlags(Qt::TextSelectableByMouse);
}

static QTableWidgetItem *createItem(const QString &text) {
    auto item = new QTableWidgetItem(text);
    item->setData(Qt::ToolTipRole, text);
    return item;
}

void AttackMessagePanel::updateItem() {
    myMessages->setRowCount(0);
    myAttackType = myItem->attackType();

    for (int message = 0; message < MAXATTACKMESS && attack_mess[myAttackType][message].level != -1; message++) {
        myMessages->setRowCount(message + 1);
        myMessages->setItem(message, 0, createItem(QString::number(attack_mess[myAttackType][message].level)));
        myMessages->setItem(message, 1, createItem(attack_mess[myAttackType][message].buf1));
        myMessages->setItem(message, 2, createItem(attack_mess[myAttackType][message].buf2));
        myMessages->setItem(message, 3, createItem(attack_mess[myAttackType][message].buf3));
    }
}

void AttackMessagePanel::currentCellChanged(int currentRow, int, int, int) {
    myForHitter->clear();
    myForVictim->clear();
    if (currentRow < 0 || currentRow >= NROFATTACKMESS || attack_mess[myAttackType][currentRow].level == -1) {
        return;
    }

    char buf1[MAX_BUF], buf2[MAX_BUF];
    get_attack_message_for_attack_type(attack_mess[myAttackType][currentRow].level - 1, myAttackType, "victim", buf1, buf2);
    myForHitter->setText(QString("You %1").arg(buf1));
    myForVictim->setText(QString("Hitter%1 you").arg(buf2));
}
