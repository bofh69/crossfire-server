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

#ifndef ARCHETYPE_WRAPPER_H
#define ARCHETYPE_WRAPPER_H

#include <QObject>

extern "C"
{
#include "global.h"
}
#include "CREPixmap.h"
#include "artifacts/AssetWithArtifacts.h"
#include "artifacts/ArtifactWrapper.h"

class ResourcesManager;

class ArchetypeWrapper : public AssetWithArtifacts<archetype> {
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QObject* clone READ clone)
    Q_PROPERTY(QObject *head READ head)
    Q_PROPERTY(QObject *more READ more)
    Q_PROPERTY(QString raw READ raw WRITE setRaw)

public:
    ArchetypeWrapper(AssetWrapper *parent, archetype* arch, ResourcesManager *resourcesManager);

    virtual QString displayName() const override { return myWrappedItem->name; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myWrappedItem->clone.face); }
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
    QString raw() const;
    void setRaw(const QString &raw);

protected:
    virtual void wasModified(AssetWrapper *asset, ChangeType type, int extra) override;
    bool appearsOnTreasureList() const;

    std::string myRaw;
};

#endif // ARCHETYPE_WRAPPER_H
