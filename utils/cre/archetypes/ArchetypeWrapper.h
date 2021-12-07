#ifndef ARCHETYPE_WRAPPER_H
#define ARCHETYPE_WRAPPER_H

#include <QObject>

extern "C"
{
#include "global.h"
}
#include "assets/AssetWrapper.h"
#include "CREPixmap.h"

class ResourcesManager;

class ArchetypeWrapper : public AssetTWrapper<archetype> {
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QObject* clone READ clone)
    Q_PROPERTY(QObject *head READ head)
    Q_PROPERTY(QObject *more READ more)

public:
    ArchetypeWrapper(AssetWrapper *parent, archetype* arch, ResourcesManager *resourcesManager);

    virtual QString displayName() const override { return myItem->name; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myItem->clone.face); }
    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int index) override;
    virtual int childIndex(AssetWrapper *child) override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    virtual bool canDrag() const override { return true; }
    virtual void drag(QMimeData *) const override;

    QString name() const;
    QObject* clone();
    QObject *head() const;
    QObject *more() const;

protected:
    ResourcesManager *myResourcesManager;
};

#endif // ARCHETYPE_WRAPPER_H
