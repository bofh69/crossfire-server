#include "RegionWrapper.h"
#include "../ResourcesManager.h"
#include "../CREMapInformationManager.h"
#include "../scripts/ScriptFile.h"
#include "../archetypes/ArchetypeWrapper.h"

RegionWrapper::RegionWrapper(AssetWrapper *parent, region *reg, ResourcesManager *resources) : AssetTWrapper(parent, "Region", reg),
        myResourcesManager(resources) {
    connect(resources->getMapInformationManager(), SIGNAL(mapAdded(CREMapInformation *)), this, SLOT(mapAdded(CREMapInformation *)));
}

int RegionWrapper::childrenCount() const {
    return myMaps.size();
}

AssetWrapper *RegionWrapper::child(int index) {
    return myMaps[index];
}

int RegionWrapper::childIndex(AssetWrapper *child) {
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
            || dynamic_cast<const ScriptFile *>(asset)
            || dynamic_cast<const ArchetypeWrapper *>(asset)
            ) ? ChildrenMayUse : DoesntUse;
}

void RegionWrapper::mapAdded(CREMapInformation *map) {
    if (map->region() == myWrappedItem->name) {
        markModified(BeforeLayoutChange);
        myMaps = myResourcesManager->getMapInformationManager()->getMapsForRegion(myWrappedItem->name);
        markModified(AfterLayoutChange);
    }
}
