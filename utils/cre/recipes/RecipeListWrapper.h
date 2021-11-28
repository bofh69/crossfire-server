#ifndef RECIPE_LIST_WRAPPER_H
#define RECIPE_LIST_WRAPPER_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"

class ResourcesManager;

class RecipeListWrapper : public AssetTWrapper<const recipelist> {
public:
    RecipeListWrapper(AssetWrapper *parent, const recipelist *list, ResourcesManager *resources);

    virtual QString displayName() const override { return myItem->items ? tr("%1 ingredients").arg(myItem->items->ingred_count) : "??"; }
    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int) override;
    virtual int childIndex(AssetWrapper *) override;

protected:
    ResourcesManager *myResourcesManager;
};

#endif // RECIPE_LIST_WRAPPER_H
