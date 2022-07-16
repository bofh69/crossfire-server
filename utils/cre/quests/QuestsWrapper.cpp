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

#include "QuestsWrapper.h"
#include "AssetsManager.h"
#include "ResourcesManager.h"

#include <QtWidgets>

/**
 * Helper dialog to input parameters for a new quest.
 */
class AddQuestDialog : public QDialog {
public:
    /**
     * Standard constructor.
     * @param resources resources manager to inform of new asset.
     */
    AddQuestDialog(ResourcesManager *resources) {
        setModal(true);
        setWindowTitle(tr("Add quest"));
        QGridLayout *layout = new QGridLayout(this);

        myMapsDirectory = tr("%1/%2/").arg(settings.datadir, settings.mapdir);

        layout->addWidget(new QLabel(tr("Quest code:")), 1, 0);
        layout->addWidget(myCode = new QLineEdit(this), 1, 1, 1, 2);

        QRadioButton *existing = new QRadioButton(tr("Add to an existing quest file:"), this);
        layout->addWidget(existing, 2, 0);
        myExistingFile = new QComboBox(this);
        auto origins = resources->questOrigins();
        for (auto &file : origins) {
            myExistingFile->addItem(QString::fromStdString(file.first));
        }
        layout->addWidget(myExistingFile, 2, 1, 1, 2);

        QRadioButton *nqf = new QRadioButton(tr("Create a new quest file:"), this);
        layout->addWidget(nqf, 3, 0);
        myNewFile = new QLineEdit(this);
        layout->addWidget(myNewFile, 3, 1);
        auto browse = new QPushButton(tr("Browse..."), this);
        layout->addWidget(browse, 3, 2);
        connect(browse, &QAbstractButton::clicked, [this] () {
            QFileDialog dlg;
            dlg.setDirectory(myMapsDirectory);
            dlg.setNameFilter(tr("Quest file (*.quests);;All files (*.*)"));
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
        });
        existing->click();

        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        connect(buttons, &QDialogButtonBox::accepted, [this] () { validate(); });
        layout->addWidget(buttons, 4, 0, 1, 3);
    }

    QString code() const { return myCode->text(); }
    QString file() const { return myNewFile->isEnabled() ? myNewFile->text() : myExistingFile->currentText(); }

private:
    QString myMapsDirectory;    /**< Base CF path for maps. */
    QLineEdit *myCode;          /**< Quest code. */
    QComboBox *myExistingFile;  /**< Selected current file. */
    QLineEdit *myNewFile;       /**< New file path. */

    /**
     * Check if data is complete. If yes then close the dialog, else explain to the
     * user what is wrong.
     */
    void validate() {
        if (myCode->text().isEmpty()) {
            QMessageBox::critical(this, tr("Empty required field"), tr("Please enter a quest code."));
            myCode->setFocus();
            return;
        }
        if (getManager()->quests()->find(myCode->text().toStdString())) {
            QMessageBox::critical(this, tr("Quest code already exists"), tr("The quest code you entered matches an existing quest."));
            myCode->setFocus();
            return;
        }
        if (myNewFile->isEnabled()) {
            if (myNewFile->text().isEmpty()) {
                QMessageBox::critical(this, tr("Empty quest file"), tr("Please enter a quest file to define the quest into."));
                myNewFile->setFocus();
                return;
            }
            if (!myNewFile->text().startsWith(myMapsDirectory)) {
                if (QMessageBox::question(this, tr("Quest file warning"), tr("The quest file seems to be outside the Crossfire maps directory (%1).\nThis means the quest will not be visible to the game.\nAre you sure you want to use this file?").arg(myMapsDirectory)) != QMessageBox::Yes) {
                    myNewFile->setFocus();
                    return;
                }
            }
            if (!myNewFile->text().endsWith(".quests")) {
                if (QMessageBox::question(this, tr("Quest file warning"), tr("The quest file does not end with '.quests'.\nThis means the quest will not be visible to the game.\nAre you sure you want to use this file?")) != QMessageBox::Yes) {
                    myNewFile->setFocus();
                    return;
                }
            }
        }
        accept();
    }
};


void QuestsWrapper::fillMenu(QMenu *menu) {
    connect(menu->addAction(tr("Add quest")), &QAction::triggered, [this] () { addQuest(); });
}

void QuestsWrapper::addQuest() {
    AddQuestDialog aqd(myResources);
    if (aqd.exec() != QDialog::Accepted) {
        return;
    }
    qDebug() << aqd.code() << aqd.file();

    quest_definition *qd = quest_create(aqd.code().toStdString().c_str());
    qd->quest_title = add_string(qd->quest_code);
    getManager()->quests()->define(qd->quest_code, qd);
    myResources->assetDefined(qd, aqd.file().toStdString());
    myResources->questModified(qd);
    markModified(BeforeChildAdd, myAssets.size());
    myAssets.push_back(myResources->wrap(qd, this));
    markModified(AfterChildAdd, myAssets.size());
}
