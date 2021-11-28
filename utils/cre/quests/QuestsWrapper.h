#ifndef QUESTS_WRAPPER_H
#define QUESTS_WRAPPER_H

#include "assets.h"
#include "AssetsManager.h"
#include "assets/AssetsCollectionWrapper.h"

class QuestsWrapper : public AssetsCollectionWrapper<quest_definition> {
public:
    QuestsWrapper(AssetWrapper *parent, ResourcesManager *resources) : AssetsCollectionWrapper(parent, "Quests", getManager()->quests(), resources) {
    }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        if (dynamic_cast<const FaceWrapper *>(asset)) {
            return ChildrenMayUse;
        }
        return DoesntUse;
    }
};

#endif /* QUESTS_WRAPPER_H */
