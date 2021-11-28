#ifndef RANDOM_MAPS_H
#define RANDOM_MAPS_H

#include "assets/AssetWrapper.h"
#include "CREMapInformationManager.h"
#include "RandomMap.h"

class RandomMaps : public AssetWrapper {
    Q_OBJECT
public:
    RandomMaps(AssetWrapper *parent, CREMapInformationManager *maps) : AssetWrapper(parent), myMaps(maps), myNeedRefresh(false) {
        connect(maps, SIGNAL(mapAdded(CREMapInformation *)), this, SLOT(mapAdded(CREMapInformation *)));
    }

    virtual QString displayName() const override { return "Random maps"; }

    virtual int childrenCount() const override { refresh(); return myCache.size(); }
    virtual AssetWrapper *child(int child) override { refresh(); return myCache[child]; }
    virtual int childIndex(AssetWrapper *child) override { refresh(); return myCache.indexOf(static_cast<RandomMap *>(child)); }

protected slots:
    void mapAdded(CREMapInformation *map) {
        if (!map->randomMaps().empty()) {
            myNeedRefresh = true;
        }
    }

private:
    CREMapInformationManager *myMaps;
    mutable bool myNeedRefresh;
    mutable QList<RandomMap *> myCache;

    void refresh() const {
        if (myNeedRefresh) {
            myNeedRefresh = false;
            myCache = myMaps->randomMaps();
        }
    }
};

#endif /* RANDOM_MAPS_H */
