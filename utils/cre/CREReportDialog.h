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

#ifndef CRE_REPORT_DIALOG_H
#define CRE_REPORT_DIALOG_H

#include <QDialog>

class QTextEdit;
class QLineEdit;
class QListWidget;

#include "CREReportDefinitionManager.h"

class CREReportDialog : public QDialog
{
    Q_OBJECT

    public:
        CREReportDialog();

    protected:
        QListWidget* myList;
        QLineEdit* myName;
        QTextEdit* myHeader;
        QTextEdit* mySort;
        QTextEdit* myDisplay;
        QTextEdit* myFooter;
        CREReportDefinitionManager myReports;
        int myReportIndex;

        virtual void accept();
        virtual void reject();

        void refreshList();
        void saveCurrentReport();

    protected slots:
        void onHelp();
        void onAdd();
        void onDelete();
        void currentRowChanged(int currentRow);
};

#endif // CRE_REPORT_DIALOG_H
