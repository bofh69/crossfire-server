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

    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QString identifier READ identifier)
    Q_PROPERTY(QString quest READ quest)
    Q_PROPERTY(int chance READ chance)
    Q_PROPERTY(const Face *face READ face)
    Q_PROPERTY(QString message READ message)

public:
    GeneralMessageWrapper(AssetWrapper *parent, GeneralMessage* message, ResourcesManager *) : AssetTWrapper(parent, "GeneralMessage", message) { }

    virtual QString displayName() const override { return (myItem->identifier && myItem->identifier[0] == '\n') ? "General message" : myItem->title; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myItem->face); }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override {
        auto face = dynamic_cast<const FaceWrapper *>(asset);
        if (face) {
            return myItem->face == face->item() ? Uses : DoesntUse;
        }
        return DoesntUse;
    }

    virtual void displayFillPanel(QWidget *panel) override {
        AssetWrapperPanel *p = static_cast<AssetWrapperPanel *>(panel);
        p->setItem(this);
    }

    QString title() const { return myItem->title; }
    QString identifier() const { return (myItem->identifier && myItem->identifier[0] != '\n') ? myItem->identifier : ""; }
    QString quest() const { return myItem->quest_code; }
    int chance() const { return myItem->chance; }
    const Face *face() const { return myItem->face; }
    QString message() const { return myItem->message; }
};

#endif // GENERAL_MESSAGE_WRAPPER_H
