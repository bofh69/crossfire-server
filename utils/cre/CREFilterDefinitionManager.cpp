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

#include "CREFilterDefinitionManager.h"
#include "CREFilterDefinition.h"
#include <QDataStream>

CREFilterDefinitionManager::CREFilterDefinitionManager()
{
}

CREFilterDefinitionManager::CREFilterDefinitionManager(const CREFilterDefinitionManager& other) : QObject()
{
    copy(other);
}

CREFilterDefinitionManager::~CREFilterDefinitionManager()
{
    qDeleteAll(myFilters);
}

void CREFilterDefinitionManager::copy(const CREFilterDefinitionManager& other)
{
    qDeleteAll(myFilters);
    myFilters.clear();
    for (int f = 0; f < other.filters().size(); f++)
    {
        myFilters.append(new CREFilterDefinition(*other.filters()[f]));
    }
}

QList<CREFilterDefinition*>& CREFilterDefinitionManager::filters()
{
    return myFilters;
}

const QList<CREFilterDefinition*>& CREFilterDefinitionManager::filters() const
{
    return myFilters;
}

QDataStream &operator<<(QDataStream &out, const CREFilterDefinitionManager &manager)
{
    out << manager.filters().size();
    for (int f = 0; f < manager.filters().size(); f++)
        out << (*manager.filters()[f]);
    return out;
}

QDataStream &operator>>(QDataStream &in, CREFilterDefinitionManager &manager)
{
    int size;
    in >> size;
    while (size > 0)
    {
        CREFilterDefinition* filter = new CREFilterDefinition();
        in >> (*filter);
        manager.filters().append(filter);
        size--;
    }
    return in;
}
