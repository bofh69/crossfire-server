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

/**
 * Macro to add a read-write property for an object flag.
 * Will generate functions to get and set the property.
 * @param name_ property name.
 * @param flag_ flag of the object, FLAG_xxx value.
 */
#define OBJ_FLAG(name_, flag_) \
  public: \
    bool name_() const { return QUERY_FLAG(myWrappedItem, flag_); } \
    void name_(bool value) const { if (value) SET_FLAG(myWrappedItem, flag_); else CLEAR_FLAG(myWrappedItem, flag_); } \
    Q_PROPERTY(bool name_ READ name_ WRITE name_)

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
    OBJ_FLAG(isMonster, FLAG_MONSTER)
    OBJ_FLAG(isAlive, FLAG_ALIVE)
    OBJ_FLAG(isGenerator, FLAG_GENERATOR)
    OBJ_FLAG(isUnaggressive, FLAG_UNAGGRESSIVE)
    ATW_RPROP(quint32, attacktype, attacktype)
    ATW_PROP(int, str, stats.Str);
    ATW_PROP(int, dex, stats.Dex);
    ATW_PROP(int, con, stats.Con);
    ATW_PROP(int, wis, stats.Wis);
    ATW_PROP(int, cha, stats.Cha);
    ATW_PROP(int, intl, stats.Int);
    ATW_PROP(int, pow, stats.Pow);
    ATW_PROP(int, wc, stats.wc)
    ATW_PROP(int, ac, stats.ac)
    ATW_PROP(int, luck, stats.luck)
    ATW_PROP(int, hp, stats.hp)
    ATW_PROP(int, maxhp, stats.maxhp)
    ATW_PROP(int, sp, stats.sp)
    ATW_PROP(int, maxsp, stats.maxsp)
    ATW_PROP(int, grace, stats.grace)
    ATW_PROP(int, maxgrace, stats.maxgrace)
    ATW_PROP(int, damage, stats.dam)
    ATW_PROP(qint64, experience, stats.exp)
    ATW_PROP(int, food, stats.food)
    Q_PROPERTY(QObject* arch READ arch)
    ATW_PROP(int, weight, weight)
    ATW_RPROP(QString, materialName, materialname)
    Q_PROPERTY(QObject* randomItems READ randomItems)
    ATW_PROP(float, speed, speed);
    Q_PROPERTY(float dps READ dps);
    Q_PROPERTY(float regen READ regen);
    ATW_RPROP(int, nrof, nrof);

    public:
        ObjectWrapper(AssetWrapper *parent, object *ob, ResourcesManager *resourcesManager);
        virtual ~ObjectWrapper();

        virtual QString displayName() const override { return name(); }

        object *getObject() { return myWrappedItem; }

        AssetWrapper* arch();
        int suggested_level() const;
        AssetWrapper *randomItems();
        const Face *face() const { return myWrappedItem->face; }
        float dps() const;
        float regen() const;

    protected:
        ResourcesManager *myResourcesManager;
};

#endif // CRE_WRAPPER_OBJECT_h
