#ifndef GENERAL_MESSAGE_WRAPPER_H
#define GENERAL_MESSAGE_WRAPPER_H

#include <QObject>

extern "C" {
#include "global.h"
}
#include "assets/AssetWrapper.h"
#include "CREPixmap.h"
#include "faces/FaceWrapper.h"
#include "assets/AssetWrapperPanel.h"

class ResourcesManager;

class GeneralMessageWrapper : public AssetTWrapper<GeneralMessage> {
    Q_OBJECT

    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString identifier READ identifier)
    Q_PROPERTY(QString quest READ quest WRITE setQuest)
    Q_PROPERTY(int chance READ chance WRITE setChance)
    Q_PROPERTY(const Face *face READ face WRITE setFace)
    Q_PROPERTY(QString message READ message WRITE setMessage)

public:
    GeneralMessageWrapper(AssetWrapper *parent, GeneralMessage* message, ResourcesManager *resources) : AssetTWrapper(parent, "GeneralMessage", message),
        myResources(resources) {
    }

    virtual QString displayName() const override { return (myWrappedItem->identifier && myWrappedItem->identifier[0] == '\n') ? "General message" : myWrappedItem->title; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myWrappedItem->face); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;
    virtual void wasModified(AssetWrapper *asset, ChangeType type, int extra) override;

    QString title() const { return myWrappedItem->title; }
    void setTitle(const QString &title);
    QString identifier() const { return (myWrappedItem->identifier && myWrappedItem->identifier[0] != '\n') ? myWrappedItem->identifier : ""; }
    QString quest() const { return myWrappedItem->quest_code; }
    void setQuest(const QString &quest);
    int chance() const { return myWrappedItem->chance; }
    void setChance(int chance);
    const Face *face() const { return myWrappedItem->face; }
    void setFace(const Face *face);
    QString message() const { return myWrappedItem->message; }
    void setMessage(const QString &message);

protected:
    ResourcesManager *myResources;
};

#endif // GENERAL_MESSAGE_WRAPPER_H
