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

#ifndef SCRIPT_FILE_MANAGER_H
#define	SCRIPT_FILE_MANAGER_H

#include <QHash>
#include "assets/AssetWrapper.h"
#include "scripts/ScriptFile.h"

class HookInformation;
class CREMapInformation;

/**
 * Manage scripts for items.
 */
class ScriptFileManager : public AssetWrapper {
    Q_OBJECT

    public:
        ScriptFileManager(AssetWrapper *parent);
        virtual ~ScriptFileManager();

        virtual QString displayName() const override { return tr("Scripts"); }
        virtual int childrenCount() const { return myScripts.size(); }
        virtual AssetWrapper *child(int child) { return myScripts.values()[child]; }
        virtual int childIndex(AssetWrapper *child) { return myScripts.values().indexOf(dynamic_cast<ScriptFile *>(child)); }

        QList<ScriptFile*> scriptsForMap(CREMapInformation* map);

        /**
         * Get information about a script path.
         * @param path full script path.
         * @return information about the script, never NULL.
         */
        ScriptFile* getFile(const QString& path);

        /**
         * Remove scripts linked to a map.
         * @param map map to remove scripts of.
         */
        void removeMap(CREMapInformation* map);

        QList<ScriptFile*> scripts() const;

    public slots:
        void addHook(const QString &file, HookInformation *hook);

    private:
      QHash<QString, ScriptFile*> myScripts;
};

#endif	/* SCRIPT_FILE_MANAGER_H */
