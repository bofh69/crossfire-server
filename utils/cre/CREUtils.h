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

#ifndef CREUTILS_H
#define CREUTILS_H

#include <QtWidgets>

extern "C" {
#include "global.h"
#include "artifact.h"
#include "treasure.h"
#include "image.h"
#include "quest.h"
}

class CREMapInformation;
class Quest;
class MessageFile;
class ScriptFile;

class CREUtils
{
    public:
        static QTreeWidgetItem* archetypeNode(QTreeWidgetItem* parent);
        static QTreeWidgetItem* archetypeNode(const archt* arch, QTreeWidgetItem* parent);

        static QTreeWidgetItem* objectNode(const object* op, QTreeWidgetItem* parent);

        static QTreeWidgetItem* treasureNode(QTreeWidgetItem* parent);
        static QTreeWidgetItem* treasureNode(const treasurelist* list, QTreeWidgetItem* parent);
        static QTreeWidgetItem* treasureNode(const treasure* treasure, const treasurelist* list, QTreeWidgetItem* parent);

        static QTreeWidgetItem* formulaeNode(const recipe* recipe, QTreeWidgetItem* parent);

        static QTreeWidgetItem* faceNode(QTreeWidgetItem* parent);
        static QTreeWidgetItem* faceNode(const Face* face, QTreeWidgetItem* parent);

        static QTreeWidgetItem* mapNode(QTreeWidgetItem *parent);
        static QTreeWidgetItem* mapNode(const CREMapInformation* map, QTreeWidgetItem *parent);

        static void addCountSuffix(QTreeWidgetItem *item, int column = 0);
};

#endif // CREUTILS_H
