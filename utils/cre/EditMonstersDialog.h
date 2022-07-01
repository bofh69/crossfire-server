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

#ifndef EDITMONSTERSDIALOG_H
#define EDITMONSTERSDIALOG_H

#include <QDialog>

class ArchetypesModel;
class ResourcesManager;
struct archt;

/**
 * Display a grid of all monsters and allow edition of various fields.
 */
class EditMonstersDialog : public QDialog {
    Q_OBJECT

public:
    EditMonstersDialog(ResourcesManager *resources);
};

#endif /* EDITMONSTERSDIALOG_H */
