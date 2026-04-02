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

#include "global.h"
#include "image.h"

class CREUtils
{
    public:
        static QTreeWidgetItem* archetypeNode(const archetype* arch, QTreeWidgetItem* parent);

        static QTreeWidgetItem* objectNode(const object* op, QTreeWidgetItem* parent);

        static QTreeWidgetItem* faceNode(QTreeWidgetItem* parent);
        static QTreeWidgetItem* faceNode(const Face* face, QTreeWidgetItem* parent);

        static std::unique_ptr<QMutexLocker> lockCrossfireData();
};

#endif // CREUTILS_H
