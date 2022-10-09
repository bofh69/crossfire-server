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

#include "QuestComboBox.h"
#include "global.h"
#include "quest.h"
#include "CREPixmap.h"
#include "AssetsManager.h"
#include "assets.h"

QuestComboBox::QuestComboBox(QWidget* parent, bool allowNone) : QComboBox(parent)
{
    QStringList names;
    if (allowNone)
    {
        names << "(none)";
        addItem(QIcon(), "(none)", QVariant::fromValue<void*>(nullptr));
    }

    getManager()->quests()->each([this, &names] (const quest_definition* quest) {
        addItem(quest->face ? CREPixmap::getIcon(quest->face->number) : QIcon(), quest->quest_code, QVariant::fromValue(static_cast<void*>(const_cast<quest_definition *>(quest))));
        names << quest->quest_code;
    });

    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setCompleter(new QCompleter(names, this));
    completer()->setFilterMode(Qt::MatchContains);
    completer()->setCaseSensitivity(Qt::CaseInsensitive);
}

QuestComboBox::~QuestComboBox()
{
}

void QuestComboBox::setQuest(const quest_definition* quest)
{
    setCurrentIndex(findData(QVariant::fromValue(static_cast<void*>(const_cast<quest_definition *>(quest)))));
}

const quest_definition *QuestComboBox::quest() const
{
    return static_cast<const quest_definition *>(currentData().value<void*>());
}
