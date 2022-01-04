#ifndef GENERAL_MESSAGES_WRAPPER_H
#define GENERAL_MESSAGES_WRAPPER_H

#include "assets.h"
extern "C" {
#include "book.h"
}
#include "AssetsManager.h"
#include "assets/AssetsCollectionWrapper.h"
#include "faces/FaceWrapper.h"
#include "quests/QuestWrapper.h"

class ResourcesManager;

class GeneralMessagesWrapper : public AssetsCollectionWrapper<GeneralMessage> {
    Q_OBJECT

public:
    GeneralMessagesWrapper(AssetWrapper *parent, ResourcesManager *resources)
        : AssetsCollectionWrapper(parent, "General messages", getManager()->messages(), resources, tr("Display all general messages.")) {
    }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        return dynamic_cast<const FaceWrapper *>(asset)
                || dynamic_cast<const QuestWrapper *>(asset)
                ? ChildrenMayUse : DoesntUse;
    }
};

#endif // GENERAL_MESSAGES_WRAPPER_H
