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

#ifndef TREASURE_LIST_COMBOBOX_H
#define TREASURE_LIST_COMBOBOX_H

#include <QtWidgets>

extern "C" {
#include "global.h"
}

/**
 * Combobox displaying a list of treasure lists, allowing selection and search.
 */
class TreasureListComboBox : public QComboBox {
    Q_OBJECT

    Q_PROPERTY(const treasurelist *list READ list WRITE setList)

 public:
    /**
     * Standard constructor.
     * @param parent ancestor of this widget.
     * @param allowNone whether to allow "(none)" treasure list.
     */
    TreasureListComboBox(QWidget *parent, bool allowNone);

    /**
     * Set the selected list.
     * @param list list to select.
     */
    void setList(const treasurelist *list);
    /**
     * Get the selected treasure list.
     * @return selected face, nullptr if no list is selected.
     */
    const treasurelist *list() const;
};

#endif /* TREASURE_LIST_COMBOBOX_H */
