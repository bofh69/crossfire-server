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

#include <Qt>

#include "AnimationControl.h"
#include "AnimationWidget.h"
#include "face.h"

AnimationControl::AnimationControl(QWidget* parent) : QWidget(parent)
{
    myAnimation = NULL;
    myStep = 0;
    myLastStep = 0;

    AnimationWidget* widget;
    QGridLayout* layout = new QGridLayout(this);

    for (int dir = 1; dir <= 8; dir++)
    {
        widget = new AnimationWidget(this);
        widget->setVisible(false);

        layout->addWidget(widget, 2 + freearr_y[dir], 2 + freearr_x[dir]);
        myWidgets.append(widget);
    }

    myTimer = new QTimer(this);
    connect(myTimer, SIGNAL(timeout()), this, SLOT(step()));
}

void AnimationControl::setAnimation(const Animations* animation, int facings)
{
    myTimer->stop();
    myAnimation = animation;
    display(animation, facings);
    myTimer->start(250);
}

void AnimationControl::display(const Animations* animation, int facings)
{
    myFacings = facings == -1 ? animation->facings : facings;

    int widget, widgetStep, faceCount, face, faceCurrent;

    for (widget = 0; widget < 8; widget++)
        myWidgets[widget]->setVisible(false);

    widget = 0;

    if (myFacings == 1)
    {
        widgetStep = 8;
        faceCount = animation->num_animations;
    }
    else if (myFacings == 2)
    {
        widgetStep = 4;
        faceCount = animation->num_animations / 2;
    }
    else if (myFacings == 4)
    {
        widgetStep = 2;
        faceCount = animation->num_animations / 4;
    }
    else
    {
        widgetStep = 1;
        faceCount = animation->num_animations / 8;
    }

    face = facings == -1 ? 0 : 1;
    while (widget < 8)
    {
        myWidgets[widget]->setVisible(true);
        QList<int> faces;
        for (faceCurrent = 0; faceCurrent < faceCount; faceCurrent++)
            faces.append(animation->faces[face++]->number);
        myWidgets[widget]->setAnimation(faces);

        widget += widgetStep;
    }
}

void AnimationControl::step()
{
    if (myAnimation == NULL)
        return;

    for (int w = 0; w < myWidgets.size(); w++)
        myWidgets[w]->step();
}
