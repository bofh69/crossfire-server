#include "RegionsWrapper.h"
#include "../ResourcesManager.h"
#include "../scripts/ScriptFile.h"
#include "../archetypes/ArchetypeWrapper.h"

RegionsWrapper::RegionsWrapper(AssetWrapper *parent, ResourcesManager *resources) : AssetWrapper(parent) {
    for (region* reg = first_region; reg; reg = reg->next) {
        myRegions.push_front(resources->wrap(reg, this));
    }
    qSort(myRegions.begin(), myRegions.end(), compareByDisplayName);
    setProperty(tipProperty, tr("Display all maps and regions."));
}

AssetWrapper::PossibleUse RegionsWrapper::uses(const AssetWrapper *asset, std::string &) const {
    return (dynamic_cast<const QuestWrapper *>(asset)
            || dynamic_cast<const FaceWrapper *>(asset)
            || dynamic_cast<const AnimationWrapper *>(asset)
            || dynamic_cast<const ScriptFile *>(asset)
            || dynamic_cast<const ArchetypeWrapper *>(asset)
            ) ? ChildrenMayUse : DoesntUse;
}
