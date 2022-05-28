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

#ifndef _CRESCRIPTENGINE_H
#define _CRESCRIPTENGINE_H

#include <QScriptEngine>

class CREScriptEngine : public QScriptEngine
{
    public:
        CREScriptEngine();
        virtual ~CREScriptEngine();
};

#endif /* _CRESCRIPTENGINE_H */
