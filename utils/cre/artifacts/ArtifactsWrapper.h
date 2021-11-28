#ifndef ARTIFACTS_WRAPPER_H
#define ARTIFACTS_WRAPPER_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"
class ResourcesManager;

class ArtifactsWrapper : public AssetWrapper {
Q_OBJECT

public:
    ArtifactsWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager);

    virtual QString displayName() const override { return "Artifacts"; }

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int) override;
    virtual int childIndex(AssetWrapper *) override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

protected:
    ResourcesManager *myResourcesManager;
};

#endif // ARTIFACTS_WRAPPER_H
