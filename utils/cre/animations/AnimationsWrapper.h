#ifndef ANIMATIONS_WRAPPER_H
#define ANIMATIONS_WRAPPER_H

#include <QObject>

extern "C"
{
#include "global.h"
#include "face.h"
}
#include "assets.h"
#include "AssetsManager.h"
#include "../assets/AssetsCollectionWrapper.h"
#include "../faces/FaceWrapper.h"

class ResourcesManager;

class AnimationsWrapper : public AssetsCollectionWrapper<Animations> {
    Q_OBJECT

public:
    AnimationsWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager)
        : AssetsCollectionWrapper(parent, "Animations", getManager()->animations(), resourcesManager) {
        }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        return dynamic_cast<const FaceWrapper *>(asset) == nullptr ? DoesntUse : ChildrenMayUse;
    }
};

#endif // ANIMATIONS_WRAPPER_H
