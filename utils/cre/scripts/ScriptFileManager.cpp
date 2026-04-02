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

#include "ScriptFileManager.h"
#include "scripts/ScriptFile.h"
#include <QVariant>

ScriptFileManager::ScriptFileManager(AssetWrapper *parent) : AssetWrapper(parent) {
    setProperty(tipProperty, tr("Display all Python scripts used in maps."));
}

ScriptFileManager::~ScriptFileManager()
{
    qDeleteAll(myScripts.values());
}

QList<ScriptFile*> ScriptFileManager::scriptsForMap(CREMapInformation* map)
{
    QList<ScriptFile*> list;
    foreach(ScriptFile* script, myScripts.values())
    {
        if (script->forMap(map))
        {
            list.append(script);
        }
    }
    return list;
}

ScriptFile* ScriptFileManager::getFile(const QString& path)
{
    if (!myScripts.contains(path))
    {
        markModified(BeforeChildAdd, myScripts.size());
        myScripts.insert(path, new ScriptFile(this, path));
        markModified(AfterChildAdd);
    }
    return myScripts[path];
}

void ScriptFileManager::removeMap(CREMapInformation* map)
{
    QHash<QString, ScriptFile*>::iterator script = myScripts.begin();
    while (script != myScripts.end())
    {
        if ((*script)->removeMap(map))
        {
            ScriptFile* s = *script;
            script = myScripts.erase(script);
            delete s;
        }
        else
        {
            script++;
        }
    }
}

QList<ScriptFile*> ScriptFileManager::scripts() const
{
    return myScripts.values();
}

void ScriptFileManager::addHook(const QString &file, HookInformation *hook) {
    getFile(file)->addHook(hook);
}
