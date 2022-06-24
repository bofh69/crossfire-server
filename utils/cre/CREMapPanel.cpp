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

#include "CREMapPanel.h"
#include "CREMapInformation.h"
#include "CREMainWindow.h"
#include "scripts/ScriptFileManager.h"
#include "scripts/ScriptFile.h"
#include "CREPixmap.h"

#include "define.h"
#include "assets.h"
#include "AssetsManager.h"

static QHash<QString, archetype*> events;

void static fillEvents()
{
    if (!events.isEmpty())
    {
        return;
    }

    getManager()->archetypes()->each([] (archetype *arch)
    {
        if (arch->clone.type == EVENT_CONNECTOR)
        {
            events.insert(arch->name, arch);
        }
    });
}

CREMapPanel::CREMapPanel(ScriptFileManager* manager, QWidget* parent) : AssetSWrapperPanel(parent)
{
    Q_ASSERT(manager != NULL);
    myManager = manager;

    myLayout->addWidget(new QLabel(tr("Name:"), this), 0, 0);
    myName = new QLabel();
    myLayout->addWidget(myName, 0, 1);

    myExitsFrom = new QTreeWidget(this);
    myExitsFrom->setHeaderLabel(tr("Exits from this map"));
    myLayout->addWidget(myExitsFrom, 1, 0, 1, 2);

    myExitsTo = new QTreeWidget(this);
    myExitsTo->setHeaderLabel(tr("Exits leading to this map"));
    myLayout->addWidget(myExitsTo, 2, 0, 1, 2);

    myScripts = new QTreeWidget(this);
    myScripts->setHeaderLabel(tr("Scripts on this map"));
    myScripts->setIconSize(QSize(32, 32));
    myLayout->addWidget(myScripts, 3, 0, 1, 2);

    myLayout->addWidget(new QLabel(tr("Background music:"), this), 4, 0);
    myBackgroundMusic = new QLabel();
    myLayout->addWidget(myBackgroundMusic, 4, 1);

    fillEvents();
}

CREMapPanel::~CREMapPanel()
{
}

void CREMapPanel::updateItem()
{
    myName->setText(myItem->name());

    myExitsFrom->clear();
    foreach(QString path, myItem->accessedFrom())
        myExitsFrom->addTopLevelItem(new QTreeWidgetItem(QStringList(path)));

    myExitsTo->clear();
    foreach(QString path, myItem->exitsTo())
        myExitsTo->addTopLevelItem(new QTreeWidgetItem(QStringList(path)));

    myScripts->clear();
    foreach(ScriptFile* script, myManager->scriptsForMap(myItem))
    {
        foreach(HookInformation* hook, script->hooks())
        {
            if (hook->map() == myItem)
            {
                QTreeWidgetItem* child = new QTreeWidgetItem(QStringList(QString("%1 [%2, %3], %4, %5, %6").arg(hook->itemName()).arg(hook->x()).arg(hook->y()).arg(hook->eventName()).arg(hook->pluginName()).arg(script->path())));

                archt* arch = events[hook->eventName().toLower()];
                if (arch != NULL && arch->clone.face != NULL)
                {
                    child->setIcon(0, CREPixmap::getIcon(arch->clone.face->number));
                }

                myScripts->addTopLevelItem(child);
            }
        }
    }

    myBackgroundMusic->setText(myItem->backgroundMusic());
}
