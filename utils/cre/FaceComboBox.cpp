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

#include "FaceComboBox.h"
#include "CREPixmap.h"
#include "AssetsManager.h"
#include "assets.h"

FaceComboBox::FaceComboBox(QWidget* parent, bool allowNone) : QComboBox(parent)
{
    QStringList names;
    if (allowNone)
    {
        names << "(none)";
        addItem(QIcon(), "(none)", QVariant::fromValue<void*>(nullptr));
    }

    getManager()->faces()->each([this, &names] (const Face* face) {
        if (face->number == 0)
            return;
        addItem(CREPixmap::getIcon(face->number), face->name, QVariant::fromValue(static_cast<void*>(const_cast<Face*>(face))));
        names << face->name;
    });

    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setCompleter(new QCompleter(names, this));
    completer()->setFilterMode(Qt::MatchContains);
    completer()->setCaseSensitivity(Qt::CaseInsensitive);
}

FaceComboBox::~FaceComboBox()
{
}

void FaceComboBox::setFace(const Face* face)
{
    setCurrentIndex(findData(QVariant::fromValue(static_cast<void*>(const_cast<Face*>(face)))));
}

const Face* FaceComboBox::face() const
{
    return static_cast<const Face*>(currentData().value<void*>());
}
