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

#ifndef MIME_UTILS_H
#define MIME_UTILS_H

#include <QString>
#include <QList>
#include <QMimeData>
#include <QDataStream>
#include <QIODevice>
#include "AssetsCollection.h"

namespace MimeUtils {
    static const char *Archetype = "x-crossfire/archetype";
    static const char *TreasureList = "x-crossfire/treasure-list";
    static const char *QuestStep = "x-crossfire/quest-step";

    void addMime(QMimeData *data, const QString &mime, const QString &name);

    template<class T>
    static QList<T *> extract(const QMimeData *data, const QString &mime, AssetsCollection<T> *assets) {
        QList<T *> ret;
        if (data->hasFormat(mime)) {
            QByteArray ba(data->data(mime));
            QDataStream df(&ba, QIODevice::ReadOnly);
            QString name;
            while (!df.atEnd()) {
                df >> name;
                if (!name.isEmpty()) {
                    auto asset = assets->find(name.toLocal8Bit().data());
                    if (asset) {
                        ret.push_back(asset);
                    }
                }
            }
        }
        return ret;
    }

    void addQuestStep(QMimeData *data, QString code, int index);
    QList<QPair<QString, int>> extractQuestSteps(const QMimeData *data);
}

#endif /* MIME_UTILS_H */
