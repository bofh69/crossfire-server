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
