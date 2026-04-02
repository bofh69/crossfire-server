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

#ifndef CREHPBARMAKER_H
#define CREHPBARMAKER_H

#include <QDialog>
#include <QObject>

class QLineEdit;
class QSpinBox;

class CREHPBarMaker : public QDialog
{
    Q_OBJECT

    public:
        CREHPBarMaker();
        virtual ~CREHPBarMaker();

    protected:
        void adjustColor();

    protected slots:
        void makeBar();
        void browse(bool);
        void selectColor(bool);

    private:
        QLineEdit* myDestination;
        QLineEdit* myName;
        QSpinBox* myHeight;
        QSpinBox* myShift;
        QColor myColor;
        QPushButton *myColorSelect;
};

#endif /* CREHPBARMAKER_H */
