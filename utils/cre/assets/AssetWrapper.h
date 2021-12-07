#ifndef ASSETWRAPPER_H
#define ASSETWRAPPER_H

#include <QObject>
#include "../CREPanel.h"
#include <QIcon>

class QMimeData;

class AssetWrapper : public QObject {
    Q_OBJECT
public:
    enum PossibleUse { Uses, ChildrenMayUse, DoesntUse };
    enum ChangeType { AssetUpdated, BeforeChildAdd, AfterChildAdd };

    AssetWrapper(AssetWrapper *parent, const QString &panelName = "empty")
        : QObject(parent), myParent(parent), myPanelName(panelName) {
    }
    virtual ~AssetWrapper() override {};

    virtual QString displayName() const = 0;
    virtual QIcon displayIcon() const { return QIcon(); }
    virtual QString displayPanelName() const { return myPanelName; }
    virtual void displayFillPanel(QWidget *) { }
    AssetWrapper *displayParent() const { return myParent; }
    void setDisplayParent(AssetWrapper *parent) { myParent = parent; }

    virtual int childrenCount() const { return 0; }
    virtual AssetWrapper *child(int) { return nullptr; }
    virtual int childIndex(AssetWrapper *) { return -1; }

    virtual PossibleUse uses(const AssetWrapper *, std::string &) const { return DoesntUse; }

    void markModified(ChangeType change, int extra = 0) {
        emit modified();
        wasModified(this, change, extra);
    }

    virtual bool canDrag() const { return false; }
    virtual void drag(QMimeData *) const { }
    virtual bool canDrop(const QMimeData *, int) const { return false; }
    virtual void drop(const QMimeData *, int) { }

signals:
    void dataModified(AssetWrapper *asset, AssetWrapper::ChangeType type, int extra);
    void modified();

protected:
    virtual void wasModified(AssetWrapper *asset, ChangeType type, int extra) {
        if (myParent) {
            myParent->wasModified(asset, type, extra);
        } else {
            emit dataModified(asset, type, extra);
        }
    }

    AssetWrapper *myParent;
    QString myPanelName;
};

template<typename T>
class AssetTWrapper : public AssetWrapper {
public:
    AssetTWrapper(AssetWrapper *parent, const QString &panelName, T* item)
        : AssetWrapper(parent, panelName) {
        myItem = item;
    };

    const T *item() const { return myItem; }

    virtual void displayFillPanel(QWidget *panel) override {
        CRETPanel<T>* p = static_cast<CRETPanel<T>*>(panel);
        p->setItem(myItem);
    }

protected:
    T *myItem;
};

#endif /* ASSETWRAPPER_H */
