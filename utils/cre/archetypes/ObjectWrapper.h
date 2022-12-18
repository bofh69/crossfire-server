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

#ifndef CRE_WRAPPER_OBJECT_h
#define CRE_WRAPPER_OBJECT_h

#include <QObject>

#include "global.h"
#include "assets/AssetWrapper.h"
#include "treasures/TreasureListWrapper.h"

class ResourcesManager;

class ObjectWrapper : public AssetTWrapper<object> {
    Q_OBJECT

    ATW_RPROP(QString, name, name)
    ATW_RPROP(QString, race, race)
    ATW_RPROP(QString, title, title)
    ATW_RPROP(QString, slaying, slaying)
    ATW_RPROP(QString, skill, skill)
    ATW_RPROP(int, type, type)
    ATW_RPROP(int, subType, subtype)
    ATW_PROP(int, level, level)
    Q_PROPERTY(int suggested_level READ suggested_level);
    Q_PROPERTY(bool isMonster READ isMonster)
    Q_PROPERTY(bool isAlive READ isAlive)
    Q_PROPERTY(bool isGenerator READ isGenerator)
    ATW_PROP(qint64, experience, stats.exp)
    ATW_RPROP(quint32, attacktype, attacktype)
    ATW_PROP(int, ac, stats.ac)
    ATW_PROP(int, con, stats.Con)
    ATW_PROP(int, wc, stats.wc)
    Q_PROPERTY(QObject* arch READ arch)
    ATW_PROP(int, damage, stats.dam)
    ATW_PROP(int, hp, stats.hp)
    ATW_PROP(int, weight, weight)
    ATW_RPROP(QString, materialName, materialname)
    Q_PROPERTY(QObject* randomItems READ randomItems)
    ATW_PROP(float, speed, speed);
    Q_PROPERTY(float dps READ dps);
    Q_PROPERTY(float regen READ regen);

    public:
        ObjectWrapper(AssetWrapper *parent, object *ob, ResourcesManager *resourcesManager);
        virtual ~ObjectWrapper();

        virtual QString displayName() const override { return name(); }

        object *getObject() { return myWrappedItem; }

        AssetWrapper* arch();
        int suggested_level() const;
        bool isMonster() const;
        bool isAlive() const;
        bool isGenerator() const { return QUERY_FLAG(myWrappedItem, FLAG_GENERATOR); }
        AssetWrapper *randomItems();
        const Face *face() const { return myWrappedItem->face; }
        float dps() const;
        float regen() const;

    protected:
        ResourcesManager *myResourcesManager;
};

#endif // CRE_WRAPPER_OBJECT_h
