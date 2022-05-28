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

#ifndef TREASURE_LIST_WRAPPER_H
#define TREASURE_LIST_WRAPPER_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"
#include "CREPixmap.h"

class ResourcesManager;
class TreasureWrapper;

class TreasureListWrapper : public AssetTWrapper<treasurelist> {
    Q_OBJECT

    Q_PROPERTY(QString name READ displayName)
    Q_PROPERTY(int totalChance READ totalChance)
    Q_PROPERTY(int itemCount READ itemCount)
    Q_PROPERTY(bool isSingleItem READ isSingleItem WRITE setSingleItem)

public:
    TreasureListWrapper(AssetWrapper *parent, treasurelist *list, ResourcesManager *resources);

    virtual QString displayName() const override { return myWrappedItem->name; }
    virtual QIcon displayIcon() const override { return myWrappedItem->total_chance == 0 ? CREPixmap::getTreasureIcon() : CREPixmap::getTreasureOneIcon(); }

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int child) override;
    virtual int childIndex(AssetWrapper *child) override;
    virtual void removeChild(AssetWrapper *child) override;

    virtual bool canDrag() const override { return true; }
    virtual void drag(QMimeData *data) const override;
    virtual bool canDrop(const QMimeData *data, int row) const override;
    virtual void drop(const QMimeData *data, int row) override;

    int totalChance() const { return myWrappedItem->total_chance; }
    void fixTotalChance();
    bool isSingleItem() const { return myWrappedItem->total_chance != 0; }
    void setSingleItem(bool isSingle);
    int itemCount() const;

    virtual PossibleUse uses(const AssetWrapper *, std::string &) const override;

protected:
    ResourcesManager *myResources;

    virtual void wasModified(AssetWrapper *asset, ChangeType type, int extra) override;
};

#endif /* TREASURE_LIST_WRAPPER_H */
