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
}

#endif /* MIME_UTILS_H */
