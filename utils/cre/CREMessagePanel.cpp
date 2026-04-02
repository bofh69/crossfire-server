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

#include <QtWidgets>
#include "CREMessagePanel.h"
#include "CREFilterDefinition.h"
#include "MessageFile.h"
#include "CREMapInformation.h"
#include "MessageManager.h"
#include "CREMessageItemModel.h"
#include "CREMultilineItemDelegate.h"
#include "CREPrePostConditionDelegate.h"
#include "CREPlayerRepliesDelegate.h"
#include "CREStringListDelegate.h"

CREMessagePanel::CREMessagePanel(const MessageManager* manager, QWidget* parent) : AssetSWrapperPanel(parent)
{
    Q_ASSERT(manager != NULL);
    myMessageManager = manager;

    addTab(tr("Details"));

    myPath = addLineEdit(tr("Path:"), "path", false);
    addLineEdit(tr("Location:"), "location", false);

    myModel = new CREMessageItemModel(this);
    myRules = new QTableView();
    myRules->setWordWrap(true);
    myRules->setModel(myModel);
    myRules->setSelectionMode(QAbstractItemView::SingleSelection);
    myRules->setSelectionBehavior(QAbstractItemView::SelectRows);
    myRules->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    myRules->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    myRules->setItemDelegateForColumn(0, new CREMultilineItemDelegate(myRules, true, true));
    myRules->setItemDelegateForColumn(1, new CREPrePostConditionDelegate(myRules, PrePostWidget::PreConditions, manager));
    myRules->setItemDelegateForColumn(2, new CREPlayerRepliesDelegate(myRules));
    myRules->setItemDelegateForColumn(3, new CREStringListDelegate(myRules));
    myRules->setItemDelegateForColumn(4, new CREPrePostConditionDelegate(myRules, PrePostWidget::PostConditions, manager));
    myRules->setItemDelegateForColumn(5, new CREMultilineItemDelegate(myRules, true, true));
    myRules->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(myRules->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentRowChanged(const QModelIndex&, const QModelIndex&)));
    addWidget(tr("Message rules (blue: uses token set by current rule as pre-condition, red: rule that sets token for pre-condition of current rule)"), myRules, false, nullptr, nullptr);

    QHBoxLayout* buttons = new QHBoxLayout();

    QPushButton* add = new QPushButton(tr("insert rule"), this);
    buttons->addWidget(add);
    connect(add, SIGNAL(clicked(bool)), this, SLOT(onAddRule(bool)));
    QPushButton* remove = new QPushButton(tr("remove rule"), this);
    buttons->addWidget(remove);
    connect(remove, SIGNAL(clicked(bool)), this, SLOT(onDeleteRule(bool)));

    QPushButton* up = new QPushButton(tr("move up"), this);
    buttons->addWidget(up);
    connect(up, SIGNAL(clicked(bool)), this, SLOT(onMoveUp(bool)));
    QPushButton* down = new QPushButton(tr("move down"), this);
    buttons->addWidget(down);
    connect(down, SIGNAL(clicked(bool)), this, SLOT(onMoveDown(bool)));

    QPushButton* copy = new QPushButton(tr("copy"), this);
    buttons->addWidget(copy);
    connect(copy, SIGNAL(clicked(bool)), this, SLOT(onDuplicate(bool)));

    QPushButton* reset = new QPushButton(tr("reset all changes"), this);
    buttons->addWidget(reset);
    connect(reset, SIGNAL(clicked(bool)), this, SLOT(onReset(bool)));

    myLayout->addLayout(buttons, myLayout->rowCount(), 0, 1, 2);

    addTab(tr("Use"));
    myUse = new QTreeWidget(this);
    addWidget(tr("Use"), myUse, false, nullptr, nullptr);

    myOriginal = nullptr;
}

CREMessagePanel::~CREMessagePanel()
{
    delete myOriginal;
}

void CREMessagePanel::updateItem()
{
    if (!myItem) {
        return;
    }
    /* can only change path when new file is created */
    myPath->setReadOnly(myItem->path() != "<new file>");

    /* so the change handler won't do anything */
    auto save = myItem;
    myItem = NULL;
    myModel->setMessage(save);
    myItem = save;

    myUse->clear();

    QTreeWidgetItem* root = NULL;
    if (myItem->maps().length() > 0)
    {
        root = new QTreeWidgetItem(myUse, QStringList(tr("Maps")));
        root->setExpanded(true);
        foreach(CREMapInformation* map, myItem->maps())
        {
            new QTreeWidgetItem(root, QStringList(map->path()));
        }
        root = NULL;
    }

    foreach(MessageFile* file, myMessageManager->messages())
    {
        bool got = false;
        foreach(MessageRule* rule, file->rules())
        {
            QStringList includes = rule->include();
            foreach(QString include, includes)
            {
                if (include.isEmpty())
                    continue;

                if (!include.startsWith('/'))
                {
                    int last = file->path().lastIndexOf(QDir::separator());
                    if (last == -1)
                        continue;
                    include = file->path().left(last + 1) + include;
                }

                if (include == myItem->path())
                {
                    if (root == NULL)
                    {
                        root = new QTreeWidgetItem(myUse, QStringList(tr("Messages")));
                        root->setExpanded(true);
                    }

                    new QTreeWidgetItem(root, QStringList(file->path()));

                    got = true;
                    break;
                }

                if (got)
                    break;
            }
        }

        if (got)
            break;
    }

    delete myOriginal;
    myOriginal = myItem->duplicate();
}

void CREMessagePanel::currentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
    myModel->setSelectedRule(current);
}

void CREMessagePanel::onAddRule(bool)
{
    int row = myRules->selectionModel()->currentIndex().row() + 1;
    myModel->insertRows(row, 1);
    myRules->selectionModel()->select(myModel->index(row, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void CREMessagePanel::onDeleteRule(bool)
{
    myModel->removeRows(myRules->selectionModel()->currentIndex().row(), 1);
}

void CREMessagePanel::onMoveUp(bool)
{
    int index = myRules->currentIndex().row();
    if (index <= 0 || index >= myItem->rules().size())
        return;

    myModel->moveUpDown(index, true);
    myRules->setCurrentIndex(myModel->index(index - 1, 0, QModelIndex()));
}

void CREMessagePanel::onMoveDown(bool)
{
    int index = myRules->currentIndex().row();
    if (index < 0 || index >= myItem->rules().size() - 1)
        return;

    myModel->moveUpDown(index, false);
    myRules->setCurrentIndex(myModel->index(index + 1, 0, QModelIndex()));
}

void CREMessagePanel::onDuplicate(bool)
{
    int index = myRules->currentIndex().row();
    if (index < 0 || index >= myItem->rules().size())
        return;

    myModel->duplicateRow(index);
    myRules->setCurrentIndex(myModel->index(index + 1, 0, QModelIndex()));
}

void CREMessagePanel::onReset(bool)
{
    if (!myItem)
        return;
    if (QMessageBox::question(this, "Confirm reset", "Reset message to its initial values?") != QMessageBox::StandardButton::Yes)
        return;

    myItem->copy(myOriginal);
    updateItem();
}
