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

#ifndef ANIMATION_WIDGET_H_
#define ANIMATION_WIDGET_H_

#include <QObject>
#include <QtWidgets>

/**
 * Display and play an animation in a single direction.
 */
class AnimationWidget : public QWidget {
    Q_OBJECT

public:
    AnimationWidget(QWidget* parent);

    void setAnimation(QList<int> faces);
    void step();
    //virtual QSize sizeHint () const { return QSize(32, 32); }

protected:
    int myStep;
    QList<int> myFaces;

    virtual void paintEvent(QPaintEvent* event);
};

#endif // ANIMATION_WIDGET_H_
