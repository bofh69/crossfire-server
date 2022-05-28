#ifndef _CREMESSAGEPANEL_H
#define _CREMESSAGEPANEL_H

#include <QObject>
#include <QBrush>
#include "assets/AssetWrapperPanel.h"
#include "MessageFile.h"

class QLineEdit;
class QTableView;
class QTreeWidget;
class CRERulePanel;
class MessageManager;
class CREMessageItemModel;

/**
 * Display information about a NPC message file, and allow edition.
 */
class CREMessagePanel : public AssetSWrapperPanel<MessageFile>
{
    Q_OBJECT

    public:
        CREMessagePanel(const MessageManager* manager, QWidget* parent);
        virtual ~CREMessagePanel();

        virtual void updateItem() override;
        virtual void commitData() override;

    private:
        const MessageManager* myMessageManager;
        MessageFile* myOriginal;
        QLineEdit* myPath;
        QLineEdit* myLocation;
        QTableView* myRules;
        CREMessageItemModel* myModel;
        QBrush myDefaultBackground;
        QTreeWidget* myUse;

    private slots:
        void currentRowChanged(const QModelIndex& current, const QModelIndex& previous);
        void onAddRule(bool);
        void onDeleteRule(bool);
        void onMoveUp(bool);
        void onMoveDown(bool);
        void onDuplicate(bool);
        void onReset(bool);
};

#endif /* _CREMESSAGEPANEL_H */
