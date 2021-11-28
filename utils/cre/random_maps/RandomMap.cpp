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
