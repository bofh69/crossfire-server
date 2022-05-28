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

#ifndef ARCHETYPE_LIST_COMBOBOX_H
#define ARCHETYPE_LIST_COMBOBOX_H

#include <QtWidgets>

extern "C" {
#include "global.h"
}

/**
 * Combobox displaying a list of archetypes, allowing selection and search.
 */
class ArchetypeComboBox : public QComboBox {
    Q_OBJECT

    Q_PROPERTY(const archetype *arch READ arch WRITE setArch)

 public:
    /**
     * Standard constructor.
     * @param parent ancestor of this widget.
     * @param allowNone whether to allow "(none)" archetype.
     */
    ArchetypeComboBox(QWidget *parent, bool allowNone);

    /**
     * Set the selected archetype.
     * @param arch archetype to select, may be null.
     */
    void setArch(const archetype *arch);
    /**
     * Get the selected archetype.
     * @return selected face, nullptr if no list is selected.
     */
    const archetype *arch() const;
};

#endif /* ARCHETYPE_LIST_COMBOBOX_H */
