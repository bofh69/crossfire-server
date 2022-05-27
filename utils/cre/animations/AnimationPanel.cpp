#include <Qt>

extern "C" {
#include "global.h"
#include "artifact.h"
#include "object.h"
#include "face.h"
}

#include "AnimationPanel.h"
#include "AnimationControl.h"
#include "CREUtils.h"

#include "assets.h"
#include "AssetsManager.h"
#include "Archetypes.h"
#include "CREMapInformationManager.h"
#include "assets/AssetModel.h"
#include "animations/AnimationWrapper.h"

AnimationPanel::AnimationPanel(QWidget* parent, AssetModel *assets) : AssetWrapperPanel(parent) {
    myAnimation = nullptr;

    addAssetUseTree("Used by", assets, "self");

    myFaces = addWidget("", new QTreeWidget(this), false, nullptr, nullptr);
    myFaces->setColumnCount(1);
    myFaces->setHeaderLabel(tr("Faces"));
    myFaces->setIconSize(QSize(32, 32));

    myDisplay = addWidget("", new AnimationControl(this), false, nullptr, nullptr);
}

void AnimationPanel::setItem(AssetWrapper *asset) {
    AssetWrapperPanel::setItem(asset);
    auto anim = dynamic_cast<AnimationWrapper *>(asset);
    Q_ASSERT(anim);
    myAnimation = anim->wrappedItem();

    myDisplay->setAnimation(myAnimation);

    myFaces->clear();
    auto root = CREUtils::faceNode(NULL);
    myFaces->addTopLevelItem(root);
    root->setExpanded(true);

    for (int face = 0; face < anim->wrappedItem()->num_animations; face++)
    {
      CREUtils::faceNode(anim->wrappedItem()->faces[face], root);
    }
}
