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

#ifndef REGIONS_WRAPPER_H
#define REGIONS_WRAPPER_H

#include "assets/AssetWrapper.h"

class ResourcesManager;

class RegionsWrapper : public AssetWrapper {
    Q_OBJECT
public:
    RegionsWrapper(AssetWrapper *parent, ResourcesManager *resources);
    virtual QString displayName() const { return tr("Regions and maps"); }

    virtual int childrenCount() const { return myRegions.size(); }
    virtual AssetWrapper *child(int index) { return myRegions[index]; }
    virtual int childIndex(AssetWrapper *child) { return myRegions.indexOf(child); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

private:
    QVector<AssetWrapper *> myRegions;
};

#endif /* REGIONS_WRAPPER_H */
