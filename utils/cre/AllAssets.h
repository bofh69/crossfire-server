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

#ifndef ALL_ASSETS_H
#define ALL_ASSETS_H

#include "assets/AssetWrapper.h"

class ResourcesManager;
class ScriptFileManager;
class MessageManager;

class AllAssets : public AssetWrapper {
public:
    AllAssets(ResourcesManager *resources, ScriptFileManager *scripts, MessageManager *messages);

    virtual QString displayName() const { return "All assets"; }

    virtual int childrenCount() const { return myAssets.size(); }
    virtual AssetWrapper *child(int index) { return myAssets[index]; }
    virtual int childIndex(AssetWrapper *child) { return myAssets.indexOf(child); }

    AssetWrapper *regions() { return myRegions; }
    AssetWrapper *randomMaps() { return myRandomMaps; }

protected:
    QVector<AssetWrapper *> myAssets;
    AssetWrapper *myRegions;
    AssetWrapper *myRandomMaps;
};

#endif /* ALL_ASSETS_H */

