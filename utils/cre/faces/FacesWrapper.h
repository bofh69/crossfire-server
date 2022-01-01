#ifndef FACES_WRAPPER_H
#define FACES_WRAPPER_H

#include "assets.h"
#include "AssetsManager.h"
#include "assets/AssetsCollectionWrapper.h"

class FacesWrapper : public AssetsCollectionWrapper<Face> {
public:
    FacesWrapper(AssetWrapper *parent, ResourcesManager *resources) : AssetsCollectionWrapper<Face>(parent, "Faces", getManager()->faces(), resources, tr("Display all faces.")) {
    }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        if (dynamic_cast<const FaceWrapper *>(asset)) {
            return ChildrenMayUse;
        }
        return DoesntUse;
    }
};

#endif /* FACES_WRAPPER_H */
