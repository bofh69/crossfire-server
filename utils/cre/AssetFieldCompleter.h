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

#ifndef ASSET_FIELD_COMPLETER_H
#define ASSET_FIELD_COMPLETER_H

#include <QCompleter>

class QStandardItem;

/**
 * Word completer to suggest fields from assets.
 * Will correctly handle dots and such.
 * The code is heavily borrowed from https://doc.qt.io/qt-6/qtwidgets-tools-customcompleter-example.html
 */
class AssetFieldCompleter : public QCompleter {

public:
    /**
     * Standard constructor.
     * @param parent parent widget.
     */
    AssetFieldCompleter(QWidget *parent);

protected:
    virtual QStringList splitPath(const QString &path) const override;
    virtual QString pathFromIndex(const QModelIndex &index) const override;
};

#endif /* ASSET_FIELD_COMPLETER_H */
