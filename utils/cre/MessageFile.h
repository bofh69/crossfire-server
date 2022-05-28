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

#ifndef _MESSAGEFILE_H
#define _MESSAGEFILE_H

#include <QObject>
#include <QStringList>
#include "assets/AssetWrapper.h"

class CREMapInformation;

/**
 * One NPC dialog rule.
 */
class MessageRule : public QObject
{
    Q_OBJECT

    public:
        MessageRule();
        MessageRule(const MessageRule& original);
        virtual ~MessageRule();

        const QString& comment() const;
        void setComment(const QString& comment);
        const QStringList& match() const;
        QStringList& match();
        void setMatch(const QStringList& match);
        const QList<QStringList>& preconditions() const;
        void setPreconditions(const QList<QStringList>& preconditions);
        const QList<QStringList>& postconditions() const;
        void setPostconditions(const QList<QStringList>& postconditions);
        const QStringList& messages() const;
        void setMessages(const QStringList& messages);
        const QStringList& include() const;
        void setInclude(const QStringList& include);
        const QList<QStringList>& replies() const;
        void setReplies(const QList<QStringList>& replies);

        bool isModified() const;
        void setModified(bool modified = true);

    private:
        bool myIsModified;
        QString myComment;
        QStringList myMatch;
        QList<QStringList> myPreconditions;
        QList<QStringList> myPostconditions;
        QStringList myMessages;
        QStringList myInclude;
        QList<QStringList> myReplies;
};

/**
 * One NPC dialog file, mostly a collection of MessageRule.
 * Once a MessageFile is created, parseFile() must be called.
 */
class MessageFile : public AssetWrapper {
    Q_OBJECT

public:
    /**
     * Standard constructor.
     * @param parent parent of this item.
     * @param path NPC dialog path.
     */
    MessageFile(AssetWrapper *parent, const QString& path);
    virtual ~MessageFile();

    virtual QString displayName() const override { return myPath; }
    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    MessageFile* duplicate() const;
    void copy(const MessageFile* other);

    /**
     * Parse the message file.
     * @return false if an error happened, true else.
     */
    bool parseFile();

    const QString& path() const;
    void setPath(const QString& path);

    const QString& location() const;
    void setLocation(const QString& location);

    QList<MessageRule*>& rules();
    QList<CREMapInformation*>& maps();

    void save();

    bool isModified() const;
    void setModified(bool modified = true);

private:
    bool myIsModified;
    QString myPath;
    QString myLocation;
    QList<MessageRule*> myRules;
    QList<CREMapInformation*> myMaps;
};

#endif /* _MESSAGEFILE_H */
