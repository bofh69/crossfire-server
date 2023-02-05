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

#include "QuickFilterDialog.h"
#include "AssetTextEdit.h"
#include <QtWidgets>

QuickFilterDialog::QuickFilterDialog(QWidget *parent, const QString &filter) : QDialog(parent) {
    setModal(true);
    setWindowTitle(tr("Quick filter"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Quick filter:"), this));
    myEdit = new AssetTextEdit(this);
    myEdit->setText(filter);
    layout->addWidget(myEdit);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

QString QuickFilterDialog::filter() const {
    return myEdit->toPlainText();
}
