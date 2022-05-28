#ifndef ANIMATION_WRAPPER_H
#define ANIMATION_WRAPPER_H

#include <QObject>

extern "C"
{
#include "global.h"
}
#include "../assets/AssetWrapper.h"
#include "../CREPixmap.h"

class ResourcesManager;

class AnimationWrapper : public AssetTWrapper<Animations> {
    Q_OBJECT

    Q_PROPERTY(QString name READ name)

public:
    AnimationWrapper(AssetWrapper *parent, Animations* anim, ResourcesManager *resourcesManager);

    virtual QString displayName() const override { return myWrappedItem->name; }
    virtual QIcon displayIcon() const { return CREPixmap::getIcon(myWrappedItem->faces[0]); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    QString name() const { return myWrappedItem->name; }

protected:
    ResourcesManager *myResourcesManager;
};

#endif // ANIMATION_WRAPPER_H
