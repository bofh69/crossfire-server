#include "MimeUtils.h"

namespace MimeUtils {

void addMime(QMimeData *data, const QString &mime, const QString &name) {
    QByteArray ba(data->data(mime));
    QDataStream df(&ba, QIODevice::WriteOnly);
    df << QString(name);
    data->setData(mime, ba);
}

}
