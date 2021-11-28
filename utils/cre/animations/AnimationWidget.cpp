#include <Qt>

#include "AnimationWidget.h"
#include "CREPixmap.h"

AnimationWidget::AnimationWidget(QWidget* parent) : QWidget(parent)
{
    myStep = 0;
    setMinimumWidth(32);
    setMinimumHeight(32);
}

void AnimationWidget::setAnimation(QList<int> faces)
{
    myFaces = faces;
    myStep = 0;
}

void AnimationWidget::step()
{
    if (myFaces.size() == 0)
        return;

    myStep++;
    if (myStep == myFaces.size())
        myStep = 0;

    repaint();
}

void AnimationWidget::paintEvent(QPaintEvent* /*event*/)
{
    if (myStep >= myFaces.size())
        return;

    QPainter painter(this);
    CREPixmap::getIcon(myFaces[myStep]).paint(&painter, 0, 0, 32, 32);
}
