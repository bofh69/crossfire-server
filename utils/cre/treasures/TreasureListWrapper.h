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

class TreasureListWrapper : public AssetTWrapper<treasurelist> {
    Q_OBJECT

    Q_PROPERTY(QString name READ displayName)
    Q_PROPERTY(int totalChance READ totalChance)
    Q_PROPERTY(int itemCount READ itemCount)
    Q_PROPERTY(bool isSingleItem READ isSingleItem WRITE setSingleItem)

public:
    TreasureListWrapper(AssetWrapper *parent, treasurelist *list, ResourcesManager *resources);

    virtual QString displayName() const override { return myItem->name; }
    virtual QIcon displayIcon() const override { return myItem->total_chance == 0 ? CREPixmap::getTreasureIcon() : CREPixmap::getTreasureOneIcon(); }
    virtual void displayFillPanel(QWidget *panel) override;

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int child) override;
    virtual int childIndex(AssetWrapper *child) override;

    virtual bool canDrag() const override { return true; }
    virtual void drag(QMimeData *) const override;

    int totalChance() const { return myItem->total_chance; }
    void fixTotalChance();
    bool isSingleItem() const { return myItem->total_chance != 0; }
    void setSingleItem(bool isSingle);
    int itemCount() const;

protected:
    ResourcesManager *myResources;

    virtual void wasModified(AssetWrapper *asset, bool updateChildren) override;
};

#endif /* TREASURE_LIST_WRAPPER_H */
