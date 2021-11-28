#ifndef QUEST_WRAPPER_H
#define QUEST_WRAPPER_H

#include <QObject>

extern "C" {
#include "global.h"
#include "quest.h"
}
#include "assets/AssetWrapper.h"
#include "CREPixmap.h"

class ResourcesManager;

class QuestWrapper : public AssetTWrapper<quest_definition> {
    Q_OBJECT

public:
    QuestWrapper(AssetWrapper *parent, quest_definition* quest, ResourcesManager *) : AssetTWrapper(parent, "Quest", quest) { }

    virtual QString displayName() const override { return myItem->quest_title; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myItem->face); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;
};

#endif // QUEST_WRAPPER_H
