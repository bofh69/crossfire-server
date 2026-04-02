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

#ifndef CRE_FILTER_DEFINITION_MANAGER_H
#define CRE_FILTER_DEFINITION_MANAGER_H

class CREFilterDefinition;

#include <QObject>
#include <QMetaType>

class CREFilterDefinitionManager : public QObject
{
    Q_OBJECT

    public:
        CREFilterDefinitionManager();
        CREFilterDefinitionManager(const CREFilterDefinitionManager& other);
        virtual ~CREFilterDefinitionManager();

        void copy(const CREFilterDefinitionManager& other);

        QList<CREFilterDefinition*>& filters();
        const QList<CREFilterDefinition*>& filters() const;

    protected:
        QList<CREFilterDefinition*> myFilters;
};

Q_DECLARE_METATYPE(CREFilterDefinitionManager);

QDataStream &operator<<(QDataStream &out, const CREFilterDefinitionManager &manager);
QDataStream &operator>>(QDataStream &in, CREFilterDefinitionManager &manager);

#endif // CRE_FILTER_DEFINITION_MANAGER_H
