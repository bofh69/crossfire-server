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

#ifndef ANIMATION_CONTROL_H
#define ANIMATION_CONTROL_H

#include <QObject>
#include <QtWidgets>

extern "C" {
#include "global.h"
#include "face.h"
}

class AnimationWidget;

class AnimationControl : public QWidget {
    Q_OBJECT

public:
    AnimationControl(QWidget* parent);

    /**
     * Define the animation to display.
     * @param animation what to display, must not be NULL.
     * @param facings if -1 then uses the facings of the animation, else number of facings to display.
     */
    void setAnimation(const Animations* animation, int facings = -1);

protected:
    const Animations* myAnimation;
    int myStep;
    int myLastStep;
    int myFacings;

    void display(const Animations* animation, int facings = -1);
    QList<AnimationWidget*> myWidgets;
    QTimer* myTimer;

private slots:
    void step();
};

#endif // ANIMATION_CONTROL_H
