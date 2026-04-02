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

#include "SoundsDialog.h"
#include <QTableWidget>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QDir>
#include <QDialogButtonBox>
#include <map>
#include "assets.h"
#include "AssetsManager.h"

#include "GameSounds.h"

QString SoundsDialog::soundType(const QString &name) const {
    if (GameSounds::gameSounds.count(name) > 0) {
        return GameSounds::tr(GameSounds::gameSounds[name].data());
    }
    auto arch = getManager()->archetypes()->first([&] (const auto &at) {
        return (at->name == name || at->clone.name == name) && (at->clone.type == SKILL || at->clone.type == SPELL || at->clone.type == SPELL_EFFECT);
    });
    if (arch) {
        switch (arch->clone.type) {
            case SKILL:
                return tr("skill use");
            case SPELL:
                return tr("spell cast");
            case SPELL_EFFECT:
                return tr("spell effect");
            default:
                return tr("unknown type");
        }
    }
    return tr("unknown");
}

SoundsDialog::SoundsDialog(const QString &dir, QWidget *parent) : QDialog(parent) {
    setModal(true);
    setWindowTitle(tr("Sound information"));

    QVBoxLayout *l = new QVBoxLayout(this);
    QTableWidget *table = new QTableWidget();
    l->addWidget(table);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels(QStringList(tr("Event")) << tr("Volume") << tr("File") << tr("Type"));

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    l->addWidget(buttons);

    QFile file(QDir(dir).filePath("sounds.conf"));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    while(!in.atEnd()) {
        const QString line = in.readLine();
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        auto split = line.split(':');
        if (split.length() != 3) {
            continue;
        }

        int pos = table->rowCount();
        table->setRowCount(pos + 1);
        for (int s = 0; s < split.length(); s++) {
            auto item = new QTableWidgetItem(split[s]);
            table->setItem(pos, s, item);
        }
        table->setItem(pos, split.length(), new QTableWidgetItem(soundType(split[0])));
    }
}
