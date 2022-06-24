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

QTreeWidgetItem* CREUtils::archetypeNode(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(QTreeWidget::tr("Archetypes")));
    return item;
}

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

QTreeWidgetItem* CREUtils::treasureNode(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(QTreeWidget::tr("Treasures")));
    return item;
}

QTreeWidgetItem* CREUtils::treasureNode(const treasurelist* list, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(list->name));
    if (list->total_chance == 0)
        item->setIcon(0, CREPixmap::getTreasureIcon());
    else
        item->setIcon(0, CREPixmap::getTreasureOneIcon());
    return item;
}

QTreeWidgetItem* CREUtils::treasureNode(const treasure* treasure, const treasurelist* list, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item;
    if (treasure->item)
    {
        item = CREUtils::archetypeNode(treasure->item, parent);
        if (treasure->next_yes)
        {
            QTreeWidgetItem* node = new QTreeWidgetItem(item, QStringList(QTreeWidget::tr("Yes")));
            node->setIcon(0, CREPixmap::getTreasureYesIcon());
            CREUtils::treasureNode(treasure->next_yes, list, node);
        }
        if (treasure->next_no)
        {
            QTreeWidgetItem* node = new QTreeWidgetItem(item, QStringList(QTreeWidget::tr("No")));
            node->setIcon(0, CREPixmap::getTreasureNoIcon());
            CREUtils::treasureNode(treasure->next_no, list, node);
        }
    }
    else
    {
        if (treasure->name && strcmp(treasure->name, "NONE") == 0)
            item = new QTreeWidgetItem(parent, QStringList(QString(QTreeWidget::tr("Nothing"))));
        else
        {
            treasurelist* other = find_treasurelist(treasure->name);
            item = CREUtils::treasureNode(other, parent);
        }
    }

    if (list->total_chance != 0)
    {
        item->setText(0, QTreeWidget::tr("%1 (%2%3%, %4 chances on %5)")
            .arg(item->text(0))
            .arg(treasure->nrof > 0 ? QTreeWidget::tr("1 to %2, ").arg(treasure->nrof) : "")
            .arg(qRound((float)100 * treasure->chance / list->total_chance))
            .arg(treasure->chance)
            .arg(list->total_chance));
    }
    else
    {
        item->setText(0, QTreeWidget::tr("%1 (%2%3%)")
            .arg(item->text(0))
            .arg(treasure->nrof > 0 ? QTreeWidget::tr("1 to %2, ").arg(treasure->nrof) : "")
            .arg(treasure->chance));
    }

    return item;
}

QTreeWidgetItem* CREUtils::formulaeNode(const recipe* recipe, QTreeWidgetItem* parent)
{
    QString title;
    const archt* base = NULL;

    if (recipe->arch_names == 0)
    {
        title = QString("%1 (no archetype?)").arg(recipe->title);
    }
    else
    {
        base = find_archetype(recipe->arch_name[0]);
        if (!base)
        {
            title = QString("%1 (no archetype?)").arg(recipe->title);
        }
        else if (strcmp(recipe->title, "NONE") == 0)
        {
            if (base->clone.title)
                title = QString("%1 %2").arg(base->clone.name, base->clone.title);
            else
                title = base->clone.name;
        }
        else
        {
            title = QString("%1 of %2").arg(base->clone.name, recipe->title);
        }
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(title));
    const Face *face = recipe_get_face(recipe);
    if (!face && base != NULL)
    {
        face = base->clone.face;
    }
    if (face)
        item->setIcon(0, CREPixmap::getIcon(face->number));

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

QTreeWidgetItem* CREUtils::mapNode(QTreeWidgetItem *parent)
{
  return new QTreeWidgetItem(parent, QStringList(QTreeWidget::tr("Maps")));
}

QTreeWidgetItem* CREUtils::mapNode(const CREMapInformation* map, QTreeWidgetItem *parent)
{
    return new QTreeWidgetItem(parent, QStringList(QObject::tr("%1 [%2]").arg(map->name(), map->path())));
}

void CREUtils::addCountSuffix(QTreeWidgetItem *item, int column)
{
    item->setText(column, QTreeWidget::tr("%1 (%2)").arg(item->text(column)).arg(item->childCount()));
}
