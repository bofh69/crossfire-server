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

#include <QtWidgets>
#include "CREUtils.h"
#include "CREPixmap.h"
#include "CREMapInformation.h"
#include "MessageFile.h"
#include "scripts/ScriptFile.h"

#include "global.h"
#include "recipe.h"
#include "libproto.h"

QTreeWidgetItem* CREUtils::archetypeNode(const archt* arch, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(arch->name));
    item->setIcon(0, CREPixmap::getIcon(arch->clone.face ? arch->clone.face->number : 0));
    return item;
}

QTreeWidgetItem* CREUtils::objectNode(const object* op, QTreeWidgetItem* parent)
{
    char name[500];
    query_name(op, name, sizeof(name));
    QString n;
    if (op->nrof > 1)
        n.append(QString::number(op->nrof)).append(" ");
    n.append(name);
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(n));
    item->setIcon(0, CREPixmap::getIcon(op->face->number));
    return item;
}

QTreeWidgetItem* CREUtils::faceNode(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(QTreeWidget::tr("Faces")));
    return item;
}

QTreeWidgetItem* CREUtils::faceNode(const Face* face, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(face->name));
    item->setIcon(0, CREPixmap::getIcon(face->number));
    return item;
}
