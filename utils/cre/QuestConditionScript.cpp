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

#include "QuestConditionScript.h"

QuestConditionScript::QuestConditionScript(const QString& name, const QString& comment)
{
    myName = name;
    myComment = comment;
}

QuestConditionScript::~QuestConditionScript() {
}

const QString& QuestConditionScript::name() const
{
    return myName;
}

const QString& QuestConditionScript::comment() const
{
    return myComment;
}
