#ifndef ANIMATION_PANEL_H
#define ANIMATION_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"

extern "C" {
#include "global.h"
#include "image.h"
}

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
