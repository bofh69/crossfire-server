#ifndef REGIONS_WRAPPER_H
#define REGIONS_WRAPPER_H

#include "assets/AssetWrapper.h"

class ResourcesManager;

class RegionsWrapper : public AssetWrapper {
public:
    RegionsWrapper(AssetWrapper *parent, ResourcesManager *resources);
    virtual QString displayName() const { return "Regions and maps"; }

    virtual int childrenCount() const { return myRegions.size(); }
    virtual AssetWrapper *child(int index) { return myRegions[index]; }
    virtual int childIndex(AssetWrapper *child) { return myRegions.indexOf(child); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

private:
    QVector<AssetWrapper *> myRegions;
};

#endif /* REGIONS_WRAPPER_H */
