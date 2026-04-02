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

#ifndef FACEMAKERDIALOG_H
#define FACEMAKERDIALOG_H

#include <QDialog>
#include <QObject>

class QTextEdit;

#include "ResourcesManager.h"

/**
 * Utility to generate variations from one or more faces.
 */
class FaceMakerDialog : public QDialog {
    Q_OBJECT

    public:
        FaceMakerDialog(QWidget* parent, ResourcesManager* manager);

    protected slots:
        void makeFaces();

    private:
        ResourcesManager* myManager;
        QTextEdit* mySettings;

        QColor parse(const QString& color);

};

#endif /* FACEMAKERDIALOG_H */

