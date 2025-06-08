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

#include "RegionWrapper.h"
#include "../ResourcesManager.h"
#include "../CREMapInformationManager.h"
#include "../scripts/ScriptFile.h"
#include "../archetypes/ArchetypeWrapper.h"

RegionWrapper::RegionWrapper(AssetWrapper *parent, region *reg, ResourcesManager *resources) : AssetTWrapper(parent, "Region", reg),
        myResourcesManager(resources) {
    connect(resources->getMapInformationManager(), SIGNAL(finished()), this, SLOT(mapBrowsingFinished()));
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

void RegionWrapper::mapBrowsingFinished() {
    markModified(BeforeLayoutChange);
    myMaps = myResourcesManager->getMapInformationManager()->getMapsForRegion(myWrappedItem->name);
    std::sort(myMaps.begin(), myMaps.end(), [] (const auto& left, const auto& right) {
       return (left->name().compare(right->name(), Qt::CaseInsensitive) < 0) ||
               (left->name().compare(right->name(), Qt::CaseInsensitive) == 0 &&
                left->path().compare(right->path(), Qt::CaseInsensitive) < 0);
    });
    markModified(AfterLayoutChange);
}
