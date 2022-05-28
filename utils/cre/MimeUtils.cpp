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

#include "MimeUtils.h"

namespace MimeUtils {

void addMime(QMimeData *data, const QString &mime, const QString &name) {
    QByteArray ba(data->data(mime));
    QDataStream df(&ba, QIODevice::WriteOnly);
    df << QString(name);
    data->setData(mime, ba);
}

}
