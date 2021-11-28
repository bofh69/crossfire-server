#include "QuestWrapper.h"
#include "faces/FaceWrapper.h"

AssetWrapper::PossibleUse QuestWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) {
        return myItem->face == face->item() ? Uses : DoesntUse;
    }
    return DoesntUse;
}
