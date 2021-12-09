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

    virtual QString displayName() const override { return (myItem->identifier && myItem->identifier[0] == '\n') ? "General message" : myItem->title; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myItem->face); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;
    virtual void displayFillPanel(QWidget *panel) override;
    virtual void wasModified(AssetWrapper *asset, ChangeType type, int extra) override;

    QString title() const { return myItem->title; }
    void setTitle(const QString &title);
    QString identifier() const { return (myItem->identifier && myItem->identifier[0] != '\n') ? myItem->identifier : ""; }
    QString quest() const { return myItem->quest_code; }
    void setQuest(const QString &quest);
    int chance() const { return myItem->chance; }
    void setChance(int chance);
    const Face *face() const { return myItem->face; }
    void setFace(const Face *face);
    QString message() const { return myItem->message; }
    void setMessage(const QString &message);

protected:
    ResourcesManager *myResources;
};

#endif // GENERAL_MESSAGE_WRAPPER_H
