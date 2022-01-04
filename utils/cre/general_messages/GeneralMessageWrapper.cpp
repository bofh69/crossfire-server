#include "GeneralMessageWrapper.h"
#include "ResourcesManager.h"

AssetWrapper::PossibleUse GeneralMessageWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *> (asset);
    if (face) {
        return myItem->face == face->item() ? Uses : DoesntUse;
    }
    auto quest = dynamic_cast<const QuestWrapper *>(asset);
    if (quest && myItem->quest_code) {
        std::string name(myItem->quest_code);
        size_t len = strlen(quest->item()->quest_code);
        return (name.length() > len && name[len] == ' ' && name.substr(0, len) == quest->item()->quest_code) ? Uses : DoesntUse;
    }
    return DoesntUse;
}

void GeneralMessageWrapper::displayFillPanel(QWidget *panel) {
    AssetWrapperPanel *p = static_cast<AssetWrapperPanel *> (panel);
    p->setItem(this);
}

void GeneralMessageWrapper::wasModified(AssetWrapper *asset, ChangeType type, int extra) {
    myResources->generalMessageModified(myItem);
    AssetWrapper::wasModified(asset, type, extra);
}

void GeneralMessageWrapper::setTitle(const QString &title) {
    if (myItem->title != title) {
        FREE_AND_COPY_IF(myItem->title, title.toLocal8Bit().data());
        markModified(AssetUpdated);
    }
}

void GeneralMessageWrapper::setQuest(const QString &quest) {
    if (myItem->quest_code != quest) {
        FREE_AND_COPY_IF(myItem->quest_code, quest.toLocal8Bit().data());
        markModified(AssetUpdated);
    }
}

void GeneralMessageWrapper::setChance(int chance) {
    if (chance != myItem->chance) {
        myItem->chance = chance;
        markModified(AssetUpdated);
    }
}

void GeneralMessageWrapper::setFace(const Face *face) {
    if (myItem->face != face) {
        myItem->face = face;
        markModified(AssetUpdated);
    }
}

void GeneralMessageWrapper::setMessage(const QString &message) {
    auto msg(message);
    if (!msg.isEmpty() && msg[msg.size() - 1] != '\n') {
        msg += "\n";
    }
    if (myItem->message != msg) {
        FREE_AND_COPY_IF(myItem->message, msg.toLocal8Bit().data());
        markModified(AssetUpdated);
    }
}
