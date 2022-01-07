#ifndef CRECOMBATSIMULATOR_H
#define CRECOMBATSIMULATOR_H

#include <QWidget>
#include <QDialog>
#include <QtWidgets>


extern "C" {
#include "global.h"
}

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
