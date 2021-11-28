#include "RegionWrapper.h"
#include "../ResourcesManager.h"
#include "../CREMapInformationManager.h"

RegionWrapper::RegionWrapper(AssetWrapper *parent, region *reg, ResourcesManager *resources) : AssetTWrapper(parent, "Region", reg),
        myResourcesManager(resources), myNeedRefresh(false) {
    connect(resources->getMapInformationManager(), SIGNAL(mapAdded(CREMapInformation *)), this, SLOT(mapAdded(CREMapInformation *)));
}

int RegionWrapper::childrenCount() const {
    refresh();
    return myMaps.size();
}

AssetWrapper *RegionWrapper::child(int index) {
    refresh();
    return myMaps[index];
}

int RegionWrapper::childIndex(AssetWrapper *child) {
    refresh();
    for (int idx = 0; idx < myMaps.size(); idx++) {
        if (myMaps[idx] == child) {
            return idx;
        }
    }
    return -1;
}

AssetWrapper::PossibleUse RegionWrapper::uses(const AssetWrapper *asset, std::string &) const {
    return (dynamic_cast<const QuestWrapper *>(asset)
            || dynamic_cast<const FaceWrapper *>(asset)
            || dynamic_cast<const AnimationWrapper *>(asset)
            ) ? ChildrenMayUse : DoesntUse;
}

void RegionWrapper::refresh() const {
    if (myNeedRefresh) {
        myMaps = myResourcesManager->getMapInformationManager()->getMapsForRegion(myItem->name);
        myNeedRefresh = false;
    }
}

void RegionWrapper::mapAdded(CREMapInformation *map) {
    if (map->region() == myItem->name) {
        myNeedRefresh = true;
    }
}
