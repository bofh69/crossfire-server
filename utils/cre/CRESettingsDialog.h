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

#ifndef CLASS_CRE_SETTINGS_DIALOG_H
#define CLASS_CRE_SETTINGS_DIALOG_H

#include <QDialog>

class QLineEdit;
class CRESettings;

class CRESettingsDialog : public QDialog
{
    Q_OBJECT
    public:
        CRESettingsDialog(CRESettings* settings);

        QString mapCache() const;

    protected:
        QLineEdit* myMapCache;
};

#endif // CLASS_CRE_SETTINGS_DIALOG_H
