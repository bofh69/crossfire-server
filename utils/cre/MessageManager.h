#ifndef _MESSAGEMANAGER_H
#define _MESSAGEMANAGER_H

#include "assets/AssetWrapper.h"
#include "MessageFile.h"
#include <QList>

class QuestConditionScript;

/**
 * Manage NPC dialogs.
 */
class MessageManager : public AssetWrapper {
    public:
        MessageManager(AssetWrapper *parent);
        virtual ~MessageManager();

        virtual QString displayName() const override { return "NPC dialogs"; }

        virtual int childrenCount() const override { return myMessages.size(); }
        virtual AssetWrapper *child(int child) override { return myMessages[child]; }
        virtual int childIndex(AssetWrapper *child) override { return myMessages.indexOf(static_cast<MessageFile *>(child)); }

        virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

        /** Load all messages from the 'maps' directory. */
        void loadMessages();
        /** Save all messages. */
        void saveMessages();

        /**
         * Get all known NPC dialogs.
         * @return NPC dialogs.
         */
        QList<MessageFile*>& messages();
        /**
         * Get all known NPC dialogs.
         * @return NPC dialogs.
         */
        const QList<MessageFile*>& messages() const;
        /**
         * Get the dialog from a file path.
         * @param path path to get the dialog from.
         * @return dialog, null if no dialog at this path.
         */
        MessageFile* findMessage(const QString& path);

        /**
         * Get all dialog pre-conditions.
         * @return pre-conditions.
         */
        QList<QuestConditionScript*> preConditions() const;
        /**
         * Get all dialog post-conditions.
         * @return post-conditions.
         */
        QList<QuestConditionScript*> postConditions() const;

    private:
        QList<MessageFile*> myMessages;
        QList<QuestConditionScript*> myPreConditions;
        QList<QuestConditionScript*> myPostConditions;

        QString loadScriptComment(const QString& path) const;
        void loadDirectory(const QString& directory);
        void findPrePost(const QString directory, QList<QuestConditionScript*>& list);
};

#endif /* _MESSAGEMANAGER_H */
