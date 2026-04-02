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

#include "CREScriptEngine.h"

#include "global.h"
#include "attack.h"

CREScriptEngine::CREScriptEngine()
{
    QScriptValue attacks = newObject();

    for (int attack = 0; attack < NROFATTACKS; attack++)
    {
        QString name(attacktype_desc[attack]);
        name = name.replace(' ', '_');
        attacks.setProperty(name, 1 << attack);
    }

    globalObject().setProperty("AttackType", attacks, QScriptValue::ReadOnly | QScriptValue::Undeletable);
}

CREScriptEngine::~CREScriptEngine()
{
}
