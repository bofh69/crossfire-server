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

#ifndef RANDOM_MAP_H
#define	RANDOM_MAP_H

#include <QString>

#include "assets/AssetWrapper.h"
#include "CREMapInformation.h"
#include "global.h"
#include "random_maps/random_map.h"
#include "random_maps/rproto.h"

class RandomMap : public AssetWrapper
{
public:
    RandomMap(CREMapInformation* parent, int x, int y, const char* parameters);

    virtual QString displayName() const override { return myMap->path(); }

    const CREMapInformation* map() const;
    int x() const;
    int y() const;
    const RMParms* parameters() const;

private:
    CREMapInformation* myMap;
    int myX;
    int myY;
    RMParms myParameters;
};

#endif	/* RANDOM_MAP_H */
