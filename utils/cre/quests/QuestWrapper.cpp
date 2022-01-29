#include "QuestWrapper.h"
#include "faces/FaceWrapper.h"

AssetWrapper::PossibleUse QuestWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) {
        return myWrappedItem->face == face->wrappedItem() ? Uses : DoesntUse;
    }
    return DoesntUse;
}
