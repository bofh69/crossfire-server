#ifndef ARTIFACT_WRAPPER_H
#define ARTIFACT_WRAPPER_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"
#include "assets/AssetWrapperPanel.h"
class ResourcesManager;

class ArtifactWrapper : public AssetTWrapper<artifact> {
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QObject* item READ item)
    Q_PROPERTY(int chance READ chance WRITE setChance)
    Q_PROPERTY(int difficulty READ difficulty)
    Q_PROPERTY(QStringList allowed READ allowed)

public:
    ArtifactWrapper(AssetWrapper *parent, artifact *art, ResourcesManager *resourcesManager);

    virtual QString displayName() const override;
    virtual QIcon displayIcon() const override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    void setSpecificItem(const object *item) { mySpecificItem = item; }

    QString name() const;
    QObject* item();
    int chance() const;
    void setChance(int chance);
    int difficulty() const;
    QStringList allowed() const;

protected:
    ResourcesManager *myResourcesManager;
    const object *mySpecificItem;
};

#endif // ARTIFACT_WRAPPER_H
