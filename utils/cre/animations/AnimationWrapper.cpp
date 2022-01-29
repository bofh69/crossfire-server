#include "AnimationWrapper.h"
#include "../ResourcesManager.h"
#include "../CREPixmap.h"
#include "assets/AssetWrapperPanel.h"

AnimationWrapper::AnimationWrapper(AssetWrapper *parent, Animations *anim, ResourcesManager *resourcesManager)
 : AssetTWrapper(parent, "Animation", anim), myResourcesManager(resourcesManager) {
}

AssetWrapper::PossibleUse AnimationWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) {
        for (uint16_t f = 0; f < myWrappedItem->num_animations; f++) {
            if (face->wrappedItem() == myWrappedItem->faces[f]) {
                return Uses;
            }
        }
        return DoesntUse;
    }
    return DoesntUse;
}

void AnimationWrapper::displayFillPanel(QWidget *panel) {
    AssetWrapperPanel *p = static_cast<AssetWrapperPanel *>(panel);
    p->setItem(this);
}
