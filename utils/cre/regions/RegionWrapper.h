#ifndef REGION_WRAPPER_H
#define REGION_WRAPPER_H

#include "assets/AssetWrapper.h"
extern "C" {
#include "global.h"
#include "map.h"
}

#include "assets/AssetWrapperPanel.h"

class ResourcesManager;
class CREMapInformation;

class RegionWrapper : public AssetTWrapper<region> {
    Q_OBJECT
    Q_PROPERTY(QString shortName READ shortName)
    Q_PROPERTY(QString longName READ longName)
    Q_PROPERTY(QString message READ message)
    Q_PROPERTY(QString jailPath READ jailPath)
    Q_PROPERTY(int jailX READ jailX);
    Q_PROPERTY(int jailY READ jailY);

public:
    RegionWrapper(AssetWrapper *parent, region *reg, ResourcesManager *resources);

    virtual QString displayName() const override { return myWrappedItem->longname; }
    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int index) override;
    virtual int childIndex(AssetWrapper *child) override;
    virtual void displayFillPanel(QWidget *panel) override {
        static_cast<AssetWrapperPanel *>(panel)->setItem(this);
    }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    QString shortName() const { return myWrappedItem->name; }
    QString longName() const { return get_region_longname(myWrappedItem); }
    QString message() const { return get_region_msg(myWrappedItem); }
    QString jailPath() const { auto reg = myWrappedItem; while (reg && !reg->jailmap) { reg = reg->parent; } return reg ? reg->jailmap : ""; }
    int jailX() const { auto reg = myWrappedItem; while (reg && !reg->jailmap) { reg = reg->parent; } return reg ? reg->jailx : 0; }
    int jailY() const { auto reg = myWrappedItem; while (reg && !reg->jailmap) { reg = reg->parent; } return reg ? reg->jaily : 0; }

protected:
    ResourcesManager *myResourcesManager;
    QList<CREMapInformation *> myMaps;

protected slots:
    void mapAdded(CREMapInformation *map);
};

#endif /* REGION_WRAPPER_H */
