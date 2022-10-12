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
#include "HelpManager.h"
#include "CREPrePostList.h"

QuestStepPanel::QuestStepPanel(MessageManager *messages, QWidget *parent) : AssetWrapperPanel(parent) {
    addSpinBox(tr("Step:"), "step", 0, 60000, false);
    addLineEdit(tr("Description:"), "description", false);
    addCheckBox(tr("End of quest:"), "isEnd", false);
    auto w = addWidget(tr("Conditions"), new PrePostWidget(this, PrePostWidget::SetWhen, messages), false, "conditions", "data");
    connect(w, SIGNAL(dataModified()), this, SLOT(dataChanged()));
    addBottomFiller();
}

CREQuestPanel::CREQuestPanel(AssetModel *model, QWidget* parent) : AssetWrapperPanel(parent)
{
    addTab(tr("Details"));
    addLineEdit(tr("Code:"), "code");
    addLineEdit(tr("Title:"), "title", false);
    addFaceChoice(tr("Face:"), "face", false);
    addQuestChoice(tr("Parent:"), "questParent", false);
    addCheckBox(tr("Can be done multiple times"), "canRestart", false);
    addCheckBox(tr("System quest:"), "isSystem", false);
    addTextEdit(tr("Description:"), "description", false);
    addTextEdit(tr("Comment:"), "comment", false);
    addBottomFiller();

    addTab(tr("Use"));
    addAssetUseTree(QString(), model, "self");

    HelpManager::setHelpId(this, "quests");
}
