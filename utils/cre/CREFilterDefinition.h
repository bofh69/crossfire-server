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

#ifndef CRE_FILTER_DEFINITION_H
#define CRE_FILTER_DEFINITION_H

#include <QObject>
#include <QMetaType>

class CREFilterDefinition : public QObject
{
    Q_OBJECT

    public:
        CREFilterDefinition();
        CREFilterDefinition(const CREFilterDefinition& other);
        virtual ~CREFilterDefinition();

        const QString& name() const;
        void setName(const QString& name);

        const QString& filter() const;
        void setFilter(const QString& filter);

    protected:
        QString myName;
        QString myFilter;
};

Q_DECLARE_METATYPE(CREFilterDefinition);

QDataStream &operator<<(QDataStream &out, const CREFilterDefinition &filter);
QDataStream &operator>>(QDataStream &in, CREFilterDefinition &filter);

#endif // CRE_FILTER_DEFINITION_H
