#ifndef RANDOM_MAP_H
#define	RANDOM_MAP_H

#include <QString>
#include "assets/AssetWrapper.h"
#include "CREMapInformation.h"

extern "C" {
#include "global.h"
#include "random_maps/random_map.h"
#include "random_maps/rproto.h"
}

class RandomMap : public AssetWrapper
{
public:
    RandomMap(CREMapInformation* parent, int x, int y, const char* parameters);

    virtual QString displayName() const override { return myMap->path(); }

    virtual void displayFillPanel(QWidget *panel) override {
        CRETPanel<RandomMap>* p = static_cast<CRETPanel<RandomMap>*>(panel);
        p->setItem(this);
    }

    const CREMapInformation* map() const;
    int x() const;
    int y() const;
    const RMParms* parameters() const;

private:
    CREMapInformation* myMap;
    int myX;
    int myY;
    RMParms myParameters;
};

#endif	/* RANDOM_MAP_H */
