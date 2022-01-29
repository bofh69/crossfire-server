#include "ArtifactListWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "faces/FaceWrapper.h"

ArtifactListWrapper::ArtifactListWrapper(AssetWrapper *parent, const artifactlist *list, ResourcesManager *resourcesManager)
 : AssetTWrapper(parent, "ArtifactList", list), myResourcesManager(resourcesManager) {
}

int ArtifactListWrapper::childrenCount() const {
    int count = 0;
    auto item = myWrappedItem->items;
    while (item) {
        count++;
        item = item->next;
    }
    return count;
}

AssetWrapper *ArtifactListWrapper::child(int index) {
    auto item = myWrappedItem->items;
    while (index > 0) {
        index--;
        item = item->next;
    }
    return myResourcesManager->wrap(item, this);
}

int ArtifactListWrapper::childIndex(AssetWrapper *child) {
    auto item = myWrappedItem->items;
    int index = 0;
    while (item) {
        if (myResourcesManager->wrap(item, this) == child) {
            return index;
        }
        item = item->next;
        index++;
    }
    return -1;
}

AssetWrapper::PossibleUse ArtifactListWrapper::uses(const AssetWrapper *asset, std::string &) const {
    if (dynamic_cast<const FaceWrapper *>(asset) || dynamic_cast<const AnimationWrapper *>(asset)) {
        return ChildrenMayUse;
    }
    return DoesntUse;
}
