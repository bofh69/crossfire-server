#ifndef RECIPES_WRAPPER_H
#define RECIPES_WRAPPER_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"
class ResourcesManager;

class RecipesWrapper : public AssetWrapper {
Q_OBJECT

public:
    RecipesWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager);

    virtual QString displayName() const { return "Recipes"; }

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int) override;
    virtual int childIndex(AssetWrapper *) override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

protected:
    ResourcesManager *myResourcesManager;
};

#endif // ARTIFACTS_WRAPPER_H
