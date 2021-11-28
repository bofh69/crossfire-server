#include "FaceWrapper.h"
#include "../ResourcesManager.h"
#include "../CREPixmap.h"

FaceWrapper::FaceWrapper(AssetWrapper *parent, Face *face, ResourcesManager *resourcesManager)
 : AssetTWrapper(parent, "Face", face), myResourcesManager(resourcesManager) {
}

AssetWrapper::PossibleUse FaceWrapper::uses(const AssetWrapper *asset, std::string &hint) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) {
        if (myItem->smoothface == face->myItem) {
            hint = "as smoothed face";
            return Uses;
        }
    }
    return DoesntUse;
}
