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

#ifndef SOUNDS_DIALOG_H
#define SOUNDS_DIALOG_H

#include <QDialog>

class SoundsDialog : public QDialog {
    Q_OBJECT
public:
    SoundsDialog(const QString &dir, QWidget *parent);

protected:
    QString soundType(const QString &name) const;
};

#endif /* SOUNDS_DIALOG_H */
