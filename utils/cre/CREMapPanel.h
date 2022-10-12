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

#ifndef _CREMAPPANEL_H
#define _CREMAPPANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"
#include "CREMapInformation.h"

class ScriptFileManager;

/**
 * Display details about a map.
 */
class CREMapPanel : public AssetSWrapperPanel<CREMapInformation>
{
    Q_OBJECT
    public:
        CREMapPanel(ScriptFileManager* manager, QWidget* parent);
        virtual ~CREMapPanel();

        virtual void updateItem() override;

    protected:
        ScriptFileManager* myManager;
        QLabel* myName;
        QTreeWidget* myExitsTo;
        QTreeWidget* myExitsFrom;
        QTreeWidget* myScripts;
        QLabel* myBackgroundMusic;
};

#endif /* _CREMAPPANEL_H */
