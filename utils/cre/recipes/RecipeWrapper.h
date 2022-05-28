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

#ifndef CRE_WRAPPER_FORMULAE_H
#define CRE_WRAPPER_FORMULAE_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"

class ResourcesManager;

class RecipeWrapper : public AssetTWrapper<const recipe> {
    Q_OBJECT

    Q_PROPERTY(QString name READ displayName)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(int chance READ chance)
    Q_PROPERTY(int difficulty READ difficulty)
    Q_PROPERTY(int experience READ experience)
    Q_PROPERTY(QStringList archs READ archs)
    Q_PROPERTY(QStringList ingredients READ ingredients)
    Q_PROPERTY(bool transmute READ transmute)
    Q_PROPERTY(int minLevel READ minLevel)
    Q_PROPERTY(QString keycode READ keycode)
    Q_PROPERTY(int index READ index)
    Q_PROPERTY(QString skill READ skill)
    Q_PROPERTY(QString cauldron READ cauldron)
    Q_PROPERTY(int yield READ yield)

public:
    RecipeWrapper(AssetWrapper *parent, const recipe *rp, ResourcesManager *resources);

    virtual QString displayName() const override;
    virtual QIcon displayIcon() const override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &hint) const override;

    QString title() const;
    int chance() const;
    int difficulty() const;
    int experience() const;
    QStringList archs() const;
    QStringList ingredients() const;
    bool transmute() const;
    int minLevel() const;
    QString keycode() const;
    int index() const { return myWrappedItem->index; }
    QString skill() const { return myWrappedItem->skill; }
    QString cauldron() const { return myWrappedItem->cauldron; }
    int yield() const { return myWrappedItem->yield; }
};

#endif // CRE_WRAPPER_FORMULAE_H
