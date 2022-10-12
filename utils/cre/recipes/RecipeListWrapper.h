/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef RECIPE_LIST_WRAPPER_H
#define RECIPE_LIST_WRAPPER_H

#include <QObject>
#include <QStringList>

#include "global.h"
#include "assets/AssetWrapper.h"

class ResourcesManager;

class RecipeListWrapper : public AssetTWrapper<const recipelist> {
    Q_OBJECT
public:
    RecipeListWrapper(AssetWrapper *parent, const recipelist *list, ResourcesManager *resources);

    virtual QString displayName() const override { return myWrappedItem->items ? tr("%1 ingredients").arg(myWrappedItem->items->ingred_count) : "??"; }
    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int) override;
    virtual int childIndex(AssetWrapper *) override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

protected:
    ResourcesManager *myResourcesManager;
};

#endif // RECIPE_LIST_WRAPPER_H
