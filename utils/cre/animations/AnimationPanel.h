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

#ifndef ANIMATION_PANEL_H
#define ANIMATION_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "global.h"
#include "image.h"
#include "assets/AssetWrapperPanel.h"

class AnimationControl;
class AssetModel;
class UseFilterAssetModel;

class AnimationPanel : public AssetTWrapperPanel<Animations> {
    Q_OBJECT

public:
    AnimationPanel(QWidget* parent, AssetModel *assets);
    virtual void updateItem() override;

protected:
    QTreeWidget* myFaces;
    AnimationControl* myDisplay;
};

#endif // ANIMATION_PANEL_H
