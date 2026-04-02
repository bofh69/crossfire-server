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

#include "CREFilterDefinition.h"

CREFilterDefinition::CREFilterDefinition()
{
}

CREFilterDefinition::CREFilterDefinition(const CREFilterDefinition& other) : QObject()
{
    myName = other.name();
    myFilter = other.filter();
}

CREFilterDefinition::~CREFilterDefinition()
{
}

const QString& CREFilterDefinition::name() const
{
    return myName;
}

void CREFilterDefinition::setName(const QString& name)
{
    myName = name;
}

const QString& CREFilterDefinition::filter() const
{
    return myFilter;
}

void CREFilterDefinition::setFilter(const QString& filter)
{
    myFilter = filter;
}

QDataStream &operator<<(QDataStream &out, const CREFilterDefinition &filter)
{
    out << filter.name() << filter.filter();
    return out;
}

QDataStream &operator>>(QDataStream &in, CREFilterDefinition &filter)
{
    QString data;
    in >> data;
    filter.setName(data);
    in >> data;
    filter.setFilter(data);
    return in;
}
