#ifndef ASSET_WITH_ARTIFACTS_H
#define ASSET_WITH_ARTIFACTS_H

#include "assets/AssetWrapper.h"
#include "artifacts/ArtifactWrapper.h"

extern "C" {
#include "global.h"
}

class ResourcesManager;

template<class T>
class AssetWithArtifacts : public AssetTWrapper<T> {
public:
    AssetWithArtifacts(AssetWrapper *parent, const QString &panelName, T* item, ResourcesManager *resources)
        : AssetTWrapper<T>(parent, panelName, item), mySpecificItem(nullptr), myResources(resources) {
    }

    virtual ~AssetWithArtifacts() {
        for (auto art : myArtifacts) {
            delete art;
        }
    }

    virtual int childrenCount() const override {
        return myArtifacts.size();
    }

    virtual AssetWrapper *child(int child) override {
        if (child >= 0 && child < static_cast<int>(myArtifacts.size())) {
            return myArtifacts[child];
        }
        return nullptr;
    }
    virtual int childIndex(AssetWrapper *child) override {
        auto pos = std::find(myArtifacts.begin(), myArtifacts.end(), child);
        if (pos != myArtifacts.end()) {
            return (pos - myArtifacts.begin());
        }
        return -1;
    }

protected:
    const object *mySpecificItem;
    ResourcesManager *myResources;
    std::vector<ArtifactWrapper *> myArtifacts;

    void setSpecificItem(const object *item, bool notify) {
        mySpecificItem = item;
        updateArtifacts(notify);
    }

    void updateArtifacts(bool notify) {
        std::vector<ArtifactWrapper *> artifacts;
        if (mySpecificItem) {
            auto list = find_artifactlist(mySpecificItem->type);
            if (list) {
                auto art = list->items;
                while (art) {
                    if (legal_artifact_combination(mySpecificItem, art)) {
                        auto wrap = new ArtifactWrapper(this, art, myResources);
                        wrap->setSpecificItem(mySpecificItem);
                        artifacts.push_back(wrap);
                    }
                    art = art->next;
                }
            }
        }

        if (notify)
            this->markModified(AssetWrapper::BeforeLayoutChange);
        std::swap(myArtifacts, artifacts);
        if (notify)
            this->markModified(AssetWrapper::AfterLayoutChange);

        for (auto art : artifacts) {
            delete art;
        }
    }
};

#endif /* ASSET_WITH_ARTIFACTS_H */
