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

#include "CREQuestPanel.h"
#include "CREQuestItemModel.h"
#include "CREMultilineItemDelegate.h"
#include "CREMapInformationManager.h"
#include "CREMapInformation.h"
#include "MessageManager.h"
#include "CREMessagePanel.h"
#include "MessageFile.h"
#include "assets.h"
#include "AssetsManager.h"
#include "FaceComboBox.h"
#include "ResourcesManager.h"
#include "CREPrePostList.h"
#include "CREPrePostConditionDelegate.h"
#include "assets/AssetModel.h"
#include "HelpManager.h"

CREQuestPanel::CREQuestPanel(CREMapInformationManager* mapManager, MessageManager* messageManager, ResourcesManager *resources, AssetModel *model, QWidget* parent) : AssetTWrapperPanel(parent)
{
    Q_ASSERT(mapManager);
    Q_ASSERT(messageManager);
    myMapManager = mapManager;
    myMessageManager = messageManager;
    myResources = resources;

    QTabWidget* tab = new QTabWidget(this);
    myLayout->addWidget(tab);

    QWidget* details = new QWidget(this);
    tab->addTab(details, tr("Details"));

    QGridLayout* layout = new QGridLayout(details);

    int line = 1;
    layout->addWidget(new QLabel(tr("Code:"), this), line, 1);
    myCode = new QLineEdit();
    myCode->setReadOnly(true);
    layout->addWidget(myCode, line++, 2);

    layout->addWidget(new QLabel(tr("Title:"), this), line, 1);
    myTitle = new QLineEdit();
    layout->addWidget(myTitle, line++, 2);

    layout->addWidget(new QLabel(tr("Face:"), this), line, 1);
    myFace = new FaceComboBox(this, true);
    layout->addWidget(myFace, line++, 2);

    myCanRestart = new QCheckBox(tr("This quest can be done multiple times"));
    layout->addWidget(myCanRestart, line++, 1, 1, 2);

    myIsSystem = new QCheckBox(tr("System quest, not listed to players"));
    layout->addWidget(myIsSystem, line++, 1, 1, 2);

    layout->addWidget(new QLabel(tr("Parent:"), this), line, 1);
    myParent = new QComboBox(this);
    layout->addWidget(myParent, line++, 2);
    myParent->addItem(tr("(none)"));

    QStringList codes;
    getManager()->quests()->each([&] (auto quest) { codes.append(quest->quest_code); });
    codes.sort();
    myParent->addItems(codes);

    layout->addWidget(new QLabel(tr("Quest file:"), this), line, 1);
    myFile = new QComboBox(this);
    layout->addWidget(myFile, line++, 2);

    myFile->setInsertPolicy(QComboBox::InsertAlphabetically);
    myFile->setEditable(true);
    myFile->addItem("");
    auto files = myResources->questOrigins();
    for (auto file : files) {
        myFile->addItem(file.first.data());
    }

    QTabWidget *dc = new QTabWidget(details);
    myDescription = new QTextEdit(this);
    dc->addTab(myDescription, "Player description");
    myComment = new QPlainTextEdit(this);
    dc->addTab(myComment, "Developer description");

    layout->addWidget(dc, line++, 1, 1, 2);

    layout->addWidget(new QLabel(tr("Steps:"), this), line++, 1, 1, 2);

    myStepsModel = new CREQuestItemModel(this);
    connect(myStepsModel, SIGNAL(questModified(quest_definition *)), resources, SLOT(questModified(quest_definition *)));
    mySteps = new QTreeView(this);
    mySteps->setRootIsDecorated(false);
    mySteps->setWordWrap(true);
    mySteps->setModel(myStepsModel);
    mySteps->setItemDelegateForColumn(3, new CREPrePostConditionDelegate(mySteps, CREPrePostList::SetWhen, messageManager));
    mySteps->setSelectionMode(QAbstractItemView::SingleSelection);

    layout->addWidget(mySteps, line++, 1, 1, 2);

    QHBoxLayout* buttons = new QHBoxLayout();

    QPushButton* add = new QPushButton(tr("add step"), this);
    connect(add, SIGNAL(clicked(bool)), myStepsModel, SLOT(addStep(bool)));
    buttons->addWidget(add);
    QPushButton* del = new QPushButton(tr("remove step"), this);
    connect(del, SIGNAL(clicked(bool)), this, SLOT(deleteStep(bool)));
    buttons->addWidget(del);

    QPushButton* up = new QPushButton(tr("move up"), this);
    connect(up, SIGNAL(clicked(bool)), this, SLOT(moveUp(bool)));
    buttons->addWidget(up);
    QPushButton* down = new QPushButton(tr("move down"), this);
    connect(down, SIGNAL(clicked(bool)), this, SLOT(moveDown(bool)));
    buttons->addWidget(down);

    layout->addLayout(buttons, line++, 1, 1, 2);

    auto useView = new QTreeView(this);
    tab->addTab(useView, tr("Use"));
    myUse = new UseFilterAssetModel(this);
    myUse->setSourceModel(model);
    useView->setModel(myUse);

    myCurrentStep = NULL;

    HelpManager::setHelpId(this, "quests");
}

CREQuestPanel::~CREQuestPanel()
{
}

void CREQuestPanel::updateItem()
{
    myCurrentStep = NULL;

    myCode->setText(myItem->quest_code);
    myTitle->setText(myItem->quest_title);
    myFace->setFace(myItem->face);
    myCanRestart->setChecked(myItem->quest_restart);
    myIsSystem->setChecked(myItem->quest_is_system);
    myDescription->setText(myItem->quest_description);
    myComment->setPlainText(myItem->quest_comment);

    auto origin = myResources->originOfQuest(myItem);
    myFile->setEditText(QString::fromStdString(origin));
    myFile->setEnabled(origin.empty());

    if (myItem->parent)
    {
        int idx = myParent->findText(myItem->parent->quest_code);
        if (idx != -1)
            myParent->setCurrentIndex(idx);
    }
    else
        myParent->setCurrentIndex(0);

    displaySteps();

    myUse->setFilter(myResources->wrap(myItem, nullptr));
}

void CREQuestPanel::commitData()
{
    if (!myItem)
        return;

    FREE_AND_COPY(myItem->quest_code, myCode->text().toStdString().data());
    FREE_AND_COPY(myItem->quest_title, myTitle->text().toStdString().data());
    myItem->face = myFace->face();
    myItem->quest_restart = myCanRestart->isChecked();
    myItem->quest_is_system = myIsSystem->isChecked();
    FREE_AND_COPY(myItem->quest_description, myDescription->toPlainText().toStdString().data());
    FREE_AND_COPY(myItem->quest_comment, myComment->toPlainText().trimmed().toStdString().data());
    if (myResources->originOfQuest(myItem).empty() && !myFile->currentText().isEmpty()) {
        myResources->assetDefined(myItem, myFile->currentText().toStdString());
    }
    if (myParent->currentIndex() == 0)
    {
        myItem->parent = NULL;
    }
    else
        myItem->parent = getManager()->quests()->get(myParent->currentText().toStdString());

    myResources->questModified(myItem);
}

void CREQuestPanel::displaySteps()
{
    myStepsModel->setQuest(myItem);
}

void CREQuestPanel::deleteStep(bool)
{
    if (myItem == NULL)
        return;

    if (!mySteps->currentIndex().isValid())
        return;

    myStepsModel->removeRow(mySteps->currentIndex().row());
}

void CREQuestPanel::moveUp(bool)
{
    if (myItem == NULL)
        return;

    if (!mySteps->currentIndex().isValid())
        return;

    myStepsModel->moveUp(mySteps->currentIndex().row());
}

void CREQuestPanel::moveDown(bool)
{
    if (myItem == NULL)
        return;

    if (!mySteps->currentIndex().isValid())
        return;

    myStepsModel->moveDown(mySteps->currentIndex().row());
}
