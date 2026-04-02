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

#include "AssetOriginAndCreationDialog.h"
#include "global.h"

const char * const addTitles[] = {
    QT_TRANSLATE_NOOP("AssetOriginAndCreationDialog", "Quest creation"),
    QT_TRANSLATE_NOOP("AssetOriginAndCreationDialog", "Treasure creation"),
    QT_TRANSLATE_NOOP("AssetOriginAndCreationDialog", "Archetype creation"),
};
const char * const originTitles[] = {
    QT_TRANSLATE_NOOP("AssetOriginAndCreationDialog", "Quest file definition"),
    QT_TRANSLATE_NOOP("AssetOriginAndCreationDialog", "Treasure list file definition"),
    QT_TRANSLATE_NOOP("AssetOriginAndCreationDialog", "Archetype file definition"),
};
const char * const names[] = {
    QT_TRANSLATE_NOOP("AssetOriginAndCreationDialog", "quest"),
    QT_TRANSLATE_NOOP("AssetOriginAndCreationDialog", "treasure"),
    QT_TRANSLATE_NOOP("AssetOriginAndCreationDialog", "archetype"),
};
const char * const extensions[] = {
    ".quests",
    ".trs",
    ".arc",
};

// Whether this file is part of 'arch' (true) or 'maps' (false)
const bool isArch[] = {
    false,
    true,
    true,
};

AssetOriginAndCreationDialog::AssetOriginAndCreationDialog(Type type, Mode mode, const QString &code,
        const std::vector<std::string> &origins, const std::vector<std::string> &assets) : myType(type), myAssets(assets) {
    setModal(true);
    setWindowTitle(tr(mode == CreateAsset ? addTitles[type] : originTitles[type]));
    QGridLayout *layout = new QGridLayout(this);

    myRootDirectory = QString("%1/%2/").arg(settings.datadir, isArch[type] ? "arch" : settings.mapdir);

    layout->addWidget(new QLabel(tr("Code:")), 1, 0);
    layout->addWidget(myCode = new QLineEdit(this), 1, 1, 1, 2);
    if (mode == DefineOrigin) {
        myCode->setReadOnly(true);
        myCode->setText(code);
    }

    QRadioButton *existing = new QRadioButton(tr("Add to an existing %1 file:").arg(names[type]), this);
    layout->addWidget(existing, 2, 0);
    myExistingFile = new QComboBox(this);
    for (const auto &origin : origins) {
        myExistingFile->addItem(QString::fromStdString(origin));
    }
    layout->addWidget(myExistingFile, 2, 1, 1, 2);

    QRadioButton *nqf = new QRadioButton(tr("Create a new %1 file:").arg(names[type]), this);
    layout->addWidget(nqf, 3, 0);
    myNewFile = new QLineEdit(this);
    layout->addWidget(myNewFile, 3, 1);
    auto browse = new QPushButton(tr("Browse..."), this);
    layout->addWidget(browse, 3, 2);
    connect(browse, &QAbstractButton::clicked, [this] () {
        QFileDialog dlg;
        if (myNewFile->text().isEmpty()) {
            dlg.setDirectory(myRootDirectory);
        } else {
            dlg.selectFile(myNewFile->text());
        }
        dlg.setNameFilter(tr("%1 file (*%2);;All files (*.*)").arg(names[myType], extensions[myType]));
        dlg.setAcceptMode(QFileDialog::AcceptSave);
        dlg.setFileMode(QFileDialog::AnyFile);
        if (dlg.exec() == QDialog::Accepted) {
            myNewFile->setText(dlg.selectedFiles()[0]);
        }
    });

    connect(existing, &QAbstractButton::clicked, [this, browse] {
        myExistingFile->setEnabled(true);
        myNewFile->setEnabled(false);
        browse->setEnabled(false);
    });
    connect(nqf, &QAbstractButton::clicked, [this, browse] {
        myExistingFile->setEnabled(false);
        myNewFile->setEnabled(true);
        browse->setEnabled(true);
        if (myNewFile->text().isEmpty()) {
            browse->click();
        }
    });
    existing->click();

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, [this] () { validate(); });
    layout->addWidget(buttons, 4, 0, 1, 3);
}

void AssetOriginAndCreationDialog::validate() {
    if (!myCode->isReadOnly()) {
        if (myCode->text().isEmpty()) {
            QMessageBox::critical(this, tr("Empty required field"), tr("Please enter a code."));
            myCode->setFocus();
            return;
        }
        if (std::find(myAssets.cbegin(), myAssets.cend(), myCode->text().toStdString()) != myAssets.cend()) {
            QMessageBox::critical(this, tr("Code already exists"), tr("The code you entered matches an existing code."));
            myCode->setFocus();
            return;
        }
    }
    if (myNewFile->isEnabled()) {
        if (myNewFile->text().isEmpty()) {
            QMessageBox::critical(this, tr("Empty file"), tr("Please enter a file to define the %1 into.").arg(names[myType]));
            myNewFile->setFocus();
            return;
        }
        if (!myNewFile->text().startsWith(myRootDirectory)) {
            if (QMessageBox::question(this, tr("File warning"), tr("The file seems to be outside the root directory for this kind of assets (%1).\nThis means the asset will not be visible to the game.\nAre you sure you want to use this file?").arg(myRootDirectory)) != QMessageBox::Yes) {
                myNewFile->setFocus();
                return;
            }
        }
        if (!myNewFile->text().endsWith(extensions[myType])) {
            if (QMessageBox::question(this, tr("File warning"), tr("The file does not end with '%1'.\nThis means the asset will not be visible to the game.\nAre you sure you want to use this file?").arg(extensions[myType])) != QMessageBox::Yes) {
                myNewFile->setFocus();
                return;
            }
        }
    }
    accept();
}
