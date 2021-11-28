#ifndef ARTIFACT_WRAPPER_H
#define ARTIFACT_WRAPPER_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"
class ResourcesManager;

class ArtifactWrapper : public AssetTWrapper<const artifact> {
    Q_OBJECT

    Q_PROPERTY(QObject* item READ item)
    Q_PROPERTY(int chance READ chance)
    Q_PROPERTY(int difficulty READ difficulty)
    Q_PROPERTY(QStringList allowed READ allowed)

public:
    ArtifactWrapper(AssetWrapper *parent, const artifact *art, ResourcesManager *resourcesManager);

    virtual QString displayName() const override { return myItem->item->name; }
    virtual QIcon displayIcon() const override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    QObject* item();
    int chance() const;
    int difficulty() const;
    QStringList allowed() const;

protected:
    ResourcesManager *myResourcesManager;
};

#endif // ARTIFACT_WRAPPER_H
