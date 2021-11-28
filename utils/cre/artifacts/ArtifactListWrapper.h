#ifndef ARTIFACT_LIST_WRAPPER_H
#define ARTIFACT_LIST_WRAPPER_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"
class ResourcesManager;

class ArtifactListWrapper : public AssetTWrapper<const artifactlist> {
Q_OBJECT

public:
    ArtifactListWrapper(AssetWrapper *parent, const artifactlist *list, ResourcesManager *resourcesManager);

    virtual QString displayName() const override {
        auto data = get_typedata(myItem->type);
        return data ? data->name : tr("type %1").arg(myItem->type);
    }

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int) override;
    virtual int childIndex(AssetWrapper *) override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

protected:
    ResourcesManager *myResourcesManager;
};

#endif // ARTIFACT_LIST_WRAPPER_H
