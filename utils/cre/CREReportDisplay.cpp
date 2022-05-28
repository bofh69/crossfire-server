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

#include "CREReportDisplay.h"
#include <QtWidgets>

CREReportDisplay::CREReportDisplay(const QString& report, const QString& title)
{
    myReport = report;

    setWindowTitle(title);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QTextEdit* view = new QTextEdit(this);
    view->setReadOnly(true);
    view->setText(report);
    layout->addWidget(view);

    QHBoxLayout* buttons = new QHBoxLayout();
    layout->addLayout(buttons);

    QPushButton* copy = new QPushButton(tr("Copy (HTML)"), this);
    buttons->addWidget(copy);
    connect(copy, SIGNAL(clicked(bool)), this, SLOT(copyClicked(bool)));

    QPushButton* close = new QPushButton(tr("Close"), this);
    buttons->addWidget(close);
    connect(close, SIGNAL(clicked(bool)), this, SLOT(closeClicked(bool)));

    setSizeGripEnabled(true);
}

void CREReportDisplay::copyClicked(bool)
{
    QApplication::clipboard()->setText(myReport);
}

void CREReportDisplay::closeClicked(bool)
{
    accept();
}
