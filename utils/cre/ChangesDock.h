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

#ifndef CHANGES_DOCK_H
#define CHANGES_DOCK_H

#include <QDockWidget>

class QHelpEngineCore;
class QTextEdit;

/**
 * Display the list of changes since last program run.
 */
class ChangesDock : public QDockWidget {
public:
    ChangesDock(QHelpEngineCore *help, QWidget *parent);

protected:
    void helpReady(QHelpEngineCore *help, QTextEdit *edit);
};

#endif /* CHANGES_DOCK_H */
