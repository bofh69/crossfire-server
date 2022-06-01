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

#ifndef _CREQUESTPANEL_H
#define _CREQUESTPANEL_H

#include <QWidget>
#include <QtWidgets>

#include "CREFilterDialog.h"
#include "CREReportDialog.h"
#include "assets/AssetWrapperPanel.h"

class MessageManager;
class AssetModel;
class PrePostWidget;

class QuestStepPanel : public AssetWrapperPanel {
    Q_OBJECT

public:
    QuestStepPanel(MessageManager *messages, QWidget *parent);

private:
    PrePostWidget *myPrePost;
};

class CREQuestPanel : public AssetWrapperPanel
{
    Q_OBJECT

public:
    CREQuestPanel(AssetModel *model, QWidget* parent);
};

#endif /* _CREQUESTPANEL_H */
