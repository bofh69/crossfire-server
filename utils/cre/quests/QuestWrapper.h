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

class QuestStepWrapper : public AssetTWrapper<quest_step_definition> {
    Q_OBJECT

    Q_PROPERTY(int step READ step WRITE setStep)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(bool isEnd READ isEnd WRITE setEnd)
    Q_PROPERTY(QList<QStringList> conditions READ conditions WRITE setConditions)

public:
    QuestStepWrapper(AssetWrapper *parent, quest_step_definition *step, ResourcesManager *)
        : AssetTWrapper(parent, "QuestStep", step) {
    }

    virtual QString displayName() const override { return tr("%1 (%2)%3")
            .arg(myWrappedItem->step_description).arg(myWrappedItem->step).arg(myWrappedItem->is_completion_step ? " (end)" : ""); }

    virtual void fillMenu(QMenu *menu) override;
    virtual bool canDrag() const override { return true; }
    virtual void drag(QMimeData *) const override;
    virtual bool canDrop(const QMimeData *, int) const override;
    virtual void drop(const QMimeData *, int) override;

    int step() const { return myWrappedItem->step; }
    QString description() const { return myWrappedItem->step_description; }
    bool isEnd() const { return myWrappedItem->is_completion_step; }

    void setStep(int step) { myWrappedItem->step = step; markModified(AssetUpdated); }
    void setDescription(QString description) {
        FREE_AND_CLEAR_STR_IF(myWrappedItem->step_description);
        myWrappedItem->step_description = add_string(description.toLocal8Bit().data());
        markModified(AssetUpdated);
    }
    void setEnd(bool end) {
        myWrappedItem->is_completion_step = end ? 1 : 0;
        markModified(AssetUpdated);
    }

    QList<QStringList> conditions() const;
    void setConditions(const QList<QStringList> &conditions);
};

class QuestWrapper : public AssetTWrapper<quest_definition> {
    Q_OBJECT

    Q_PROPERTY(QString code READ code)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(const Face *face READ face WRITE setFace)
    Q_PROPERTY(const quest_definition *questParent READ questParent WRITE setQuestParent);
    Q_PROPERTY(bool canRestart READ canRestart WRITE setRestart)
    Q_PROPERTY(bool isSystem READ isSystem WRITE setSystem)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QString comment READ comment WRITE setComment)

public:
    QuestWrapper(AssetWrapper *parent, quest_definition* quest, ResourcesManager *resources)
        : AssetTWrapper(parent, "Quest", quest), myResources(resources) { }

    virtual QString displayName() const override { return myWrappedItem->quest_title; }
    virtual QIcon displayIcon() const override { return CREPixmap::getIcon(myWrappedItem->face); }

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int) override;
    virtual int childIndex(AssetWrapper *) override;

    virtual bool canDrop(const QMimeData *, int) const override;
    virtual void drop(const QMimeData *, int) override;
    virtual void fillMenu(QMenu *menu) override;

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    QString code() const { return myWrappedItem->quest_code; }
    QString title() const { return myWrappedItem->quest_title; }
    const Face *face() const { return myWrappedItem->face; }
    const quest_definition *questParent() const { return myWrappedItem->parent; }
    QString description() const { return myWrappedItem->quest_description; }
    QString comment() const { return myWrappedItem->quest_comment; }
    bool canRestart() const { return myWrappedItem->quest_restart; }
    bool isSystem() const { return myWrappedItem->quest_is_system; }

    void setTitle(const QString &title) {
        if (title != myWrappedItem->quest_title) {
            FREE_AND_CLEAR_STR_IF(myWrappedItem->quest_title);
            myWrappedItem->quest_title = add_string(title.toLocal8Bit().data());
            markModified(AssetUpdated);
        }
    }
    void setFace(const Face *face) {
        if (face != myWrappedItem->face) {
            myWrappedItem->face = face;
            markModified(AssetUpdated);
        }
    }
    void setQuestParent(const quest_definition *parent) {
        if (parent != myWrappedItem->parent) {
            myWrappedItem->parent = const_cast<quest_definition *>(parent);
            markModified(AssetUpdated);
        }
    }
    void setRestart(bool restart) {
        if (restart != myWrappedItem->quest_restart) {
            myWrappedItem->quest_restart = restart ? 1 : 0;
            markModified(AssetUpdated);
        }
    }
    void setSystem(bool system) {
        if (system != myWrappedItem->quest_is_system) {
            myWrappedItem->quest_is_system = system;
            markModified(AssetUpdated);
        }
    }
    void setDescription(const QString &description) {
        if (description != myWrappedItem->quest_description) {
            FREE_AND_CLEAR_STR_IF(myWrappedItem->quest_description);
            myWrappedItem->quest_description = add_string(description.toLocal8Bit().data());
            markModified(AssetUpdated);
        }
    }
    void setComment(const QString &comment) {
        if (comment != myWrappedItem->quest_comment) {
            FREE_AND_CLEAR_STR_IF(myWrappedItem->quest_comment);
            myWrappedItem->quest_comment = add_string(comment.toLocal8Bit().data());
            markModified(AssetUpdated);
        }
    }

protected:
    ResourcesManager *myResources;

    virtual void wasModified(AssetWrapper *asset, ChangeType type, int extra) override;
    void addStep();
    virtual void removeChild(AssetWrapper *child) override;
};

#endif // QUEST_WRAPPER_H
