#ifndef FACE_SETS_WRAPPER_H
#define FACE_SETS_WRAPPER_H

#include <QObject>

extern "C"
{
#include "global.h"
}
#include "assets/AssetWrapper.h"
#include "assets/AssetWrapperPanel.h"

class ResourcesManager;

class FacesetsWrapper : public AssetTWrapper<face_sets> {
    Q_OBJECT

    Q_PROPERTY(QString prefix READ prefix)
    Q_PROPERTY(QString fullname READ fullname)
    Q_PROPERTY(QString fallback READ fallback)
    Q_PROPERTY(QString size READ size)
    Q_PROPERTY(QString extension READ extension)

public:
    FacesetsWrapper(AssetWrapper *parent, face_sets* fs, ResourcesManager *) : AssetTWrapper(parent, "Faceset", fs) { }

    virtual QString displayName() const override { return myItem->prefix; }
    virtual void displayFillPanel(QWidget *panel) override {
        AssetWrapperPanel *p = static_cast<AssetWrapperPanel *>(panel);
        p->setItem(this);
    }

    QString prefix() const { return myItem->prefix; }
    QString fullname() const { return myItem->fullname; }
    QString fallback() const { return myItem->fallback ? myItem->fallback->prefix : ""; }
    QString size() const { return myItem->size; }
    QString extension() const { return myItem->extension; }
};

#endif // FACE_SETS_WRAPPER_H
