#ifndef ASSETS_COLLECTION_WRAPPER_H
#define ASSETS_COLLECTION_WRAPPER_H

#include "AssetWrapper.h"
#include "../ResourcesManager.h"
#include "AssetsCollection.h"

template<typename T>
class AssetsCollectionWrapper : public AssetWrapper {
public:
    AssetsCollectionWrapper(AssetWrapper *parent, const QString &name, AssetsCollection<T> *collection, ResourcesManager *resources)
        : AssetWrapper(parent), myName(name) {
        collection->each([&] (T *asset) {
            myAssets.append(resources->wrap(asset, this));
        });
    }

    virtual QString displayName() const { return myName; }
    virtual int childrenCount() const override { return myAssets.size(); }
    virtual AssetWrapper *child(int index) override { return myAssets[index]; }
    virtual int childIndex(AssetWrapper *child) override { return myAssets.indexOf(child); }

protected:
    QString myName;
    QVector<AssetWrapper *> myAssets;
};

#endif /* ASSETS_COLLECTION_WRAPPER_H */
