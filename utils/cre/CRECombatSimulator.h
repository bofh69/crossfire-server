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

#ifndef CRECOMBATSIMULATOR_H
#define CRECOMBATSIMULATOR_H

#include <QWidget>
#include <QDialog>
#include <QtWidgets>


#include "global.h"

class QComboBox;
class QSpinBox;
class ArchetypeComboBox;

class CRECombatSimulator : public QDialog
{
    Q_OBJECT

    public:
        CRECombatSimulator();
        virtual ~CRECombatSimulator();

    protected slots:
        void fight();

    private:
        void fight(const archetype* first, const archetype* second);

        ArchetypeComboBox* myFirst;
        ArchetypeComboBox* mySecond;
        QSpinBox* myCombats;
        QSpinBox* myMaxRounds;
        QLabel* myResultLabel;
        QLabel* myResult;

        int myFirstVictories;
        int myFirstMinHp;
        int myFirstMaxHp;
        int mySecondVictories;
        int mySecondMinHp;
        int mySecondMaxHp;
};

#endif /* CRECOMBATSIMULATOR_H */
