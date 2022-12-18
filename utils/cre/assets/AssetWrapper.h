/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef ASSETWRAPPER_H
#define ASSETWRAPPER_H

#include <QObject>
#include <QIcon>

class QMimeData;
class QMenu;

/**
 * Base class for all assets that can be displayed or edited by CRE.
 */
class AssetWrapper : public QObject {
    Q_OBJECT

    Q_PROPERTY(AssetWrapper *self READ self)
    Q_PROPERTY(QString displayName READ displayName)

public:
    enum PossibleUse { Uses, ChildrenMayUse, DoesntUse };
    enum ChangeType { AssetUpdated, BeforeChildAdd, AfterChildAdd, BeforeChildRemove, AfterChildRemove, BeforeLayoutChange, AfterLayoutChange };
    static const char *tipProperty;

    AssetWrapper(AssetWrapper *parent, const QString &panelName = "empty")
        : QObject(parent), myParent(parent), myPanelName(panelName) {
    }
    virtual ~AssetWrapper() override {};

    AssetWrapper *self() { return this; }

    virtual QString displayName() const = 0;
    virtual QIcon displayIcon() const { return QIcon(); }
    virtual QString displayPanelName() const { return myPanelName; }
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
    virtual void fillMenu(QMenu *) { }

    static bool compareByDisplayName(const AssetWrapper *left, const AssetWrapper *right) {
        return left->displayName().compare(right->displayName(), Qt::CaseInsensitive) < 0;
    }

public slots:
    virtual void removeChild(AssetWrapper *) { }

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

/**
 * Base class for assets mapping to a native Crossfire structure.
 */
template<typename T>
class AssetTWrapper : public AssetWrapper {
public:
    AssetTWrapper(AssetWrapper *parent, const QString &panelName, T* wrappedItem)
        : AssetWrapper(parent, panelName) {
        myWrappedItem = wrappedItem;
    };

    T *wrappedItem() const { return myWrappedItem; }

protected:
    T *myWrappedItem;
};

/**
 * Macro to add a read-only property from myWrappedItem.
 * Will generate a function to get the property.
 * @param type_ property type.
 * @param name_ property name.
 * @param field_ field of myWrappedItem to use.
 */
#define ATW_RPROP(type_, name_, field_) \
  public: \
    type_ name_() const { return myWrappedItem->field_; } \
    Q_PROPERTY(type_ name_ READ name_)

/**
 * Macro to add a read-write property from myWrappedItem.
 * Will generate functions to get and set the property.
 * @param type_ property type.
 * @param name_ property name.
 * @param field_ field of myWrappedItem to use.
 */
#define ATW_PROP(type_, name_, field_) \
  public: \
    type_ name_() const { return myWrappedItem->field_; } \
    void name_(const type_ &val) { myWrappedItem->field_ = val; } \
    Q_PROPERTY(type_ name_ READ name_ WRITE name_)

#endif /* ASSETWRAPPER_H */
