#ifndef ANIMATION_PANEL_H
#define ANIMATION_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "CREPanel.h"

extern "C" {
#include "global.h"
#include "image.h"
}

#include "assets/AssetWrapperPanel.h"

class AnimationControl;
class AssetModel;
class UseFilterAssetModel;

class AnimationPanel : public AssetWrapperPanel {
    Q_OBJECT

public:
    AnimationPanel(QWidget* parent, AssetModel *assets);
    virtual void setItem(AssetWrapper *asset) override;

protected:
    const Animations* myAnimation;

    QTreeView* myUsingView;
    UseFilterAssetModel *myAssets;
    QTreeWidget* myFaces;
    AnimationControl* myDisplay;
};

#endif // ANIMATION_PANEL_H
