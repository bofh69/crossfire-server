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

class TreasureListWrapper : public AssetTWrapper<const treasurelist> {
    Q_OBJECT

    Q_PROPERTY(QString name READ displayName)
    Q_PROPERTY(int total_chance READ totalChance)
    Q_PROPERTY(int item_count READ itemCount)

public:
    TreasureListWrapper(AssetWrapper *parent, const treasurelist *list, ResourcesManager *resources);

    virtual QString displayName() const override { return myItem->name; }
    virtual QIcon displayIcon() const override { return myItem->total_chance == 0 ? CREPixmap::getTreasureIcon() : CREPixmap::getTreasureOneIcon(); }

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int child) override;
    virtual int childIndex(AssetWrapper *child) override;

    int totalChance() const { return myItem->total_chance; }
    int itemCount() const;

protected:
    ResourcesManager *myResources;
};

#endif /* TREASURE_LIST_WRAPPER_H */

