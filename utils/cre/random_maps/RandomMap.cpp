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

#include "RandomMap.h"

RandomMap::RandomMap(CREMapInformation *parent, int x, int y, const char* parameters) : AssetWrapper(parent, "RandomMap")
{
    myMap = parent;
    myX = x;
    myY = y;
    memset(&myParameters, 0, sizeof myParameters);
    set_random_map_variable(&myParameters, parameters);
}

const CREMapInformation* RandomMap::map() const
{
    return myMap;
}

int RandomMap::x() const
{
    return myX;
}

int RandomMap::y() const
{
    return myY;
}

const RMParms* RandomMap::parameters() const
{
    return &myParameters;
}
