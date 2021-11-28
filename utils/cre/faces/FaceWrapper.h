#ifndef FACE_WRAPPER_H
#define FACE_WRAPPER_H

#include <QObject>

extern "C"
{
#include "global.h"
}
#include "assets/AssetWrapper.h"
#include "CREPixmap.h"

class ResourcesManager;

class FaceWrapper : public AssetTWrapper<Face> {
    Q_OBJECT

    Q_PROPERTY(QString name READ name)

public:
    FaceWrapper(AssetWrapper *parent, Face* face, ResourcesManager *resourcesManager);

    virtual QString displayName() const override { return myItem->name; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myItem); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    QString name() const { return myItem->name; }

protected:
    ResourcesManager *myResourcesManager;
};

#endif // FACE_WRAPPER_H
