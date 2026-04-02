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

#ifndef ASSET_TEXT_EDIT_H
#define ASSET_TEXT_EDIT_H

#include <QTextEdit>

class AssetFieldCompleter;

/**
 * Multiline edit with auto-completion for asset fields.
 * This class relies on @ref AssetFieldCompleter for completion.
 * The code is heavily borrowed from https://doc.qt.io/qt-6/qtwidgets-tools-customcompleter-example.html
 */
class AssetTextEdit : public QTextEdit {
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param parent parent widget.
     */
    AssetTextEdit(QWidget *parent);

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;

private:
    AssetFieldCompleter *myCompleter; /**< Completer used to suggest words. */

    /**
     * Get the complete text under the cursor, including all letters and dots before, all letters after.
     * @return test.
     */
    QString textUnderCursor() const;

private slots:
    /**
     * Insert the completed text at the cursor's position, replacing part of the contents if needed.
     * @param completion text to insert.
     */
    void insertCompletion(const QString &completion);
};

#endif /* ASSET_TEXT_EDIT_H */
