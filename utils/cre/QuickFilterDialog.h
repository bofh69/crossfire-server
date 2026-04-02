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

#ifndef QUICK_FILTER_DIALOG_H
#define QUICK_FILTER_DIALOG_H

#include <QDialog>

class AssetTextEdit;

/**
 * Dialog for the "quick filter", using a field completer.
 */
class QuickFilterDialog : public QDialog {
    Q_OBJECT
public:
    /**
     * Standard constructor.
     * @param parent parent widget.
     * @param filter default filter to display.
     */
    QuickFilterDialog(QWidget *parent, const QString &filter);

    /**
     * Get the input filter.
     * @return filter.
     */
    QString filter() const;

private:
    AssetTextEdit *myEdit;  /**< Input zone. */
};

#endif /* QUICK_FILTER_DIALOG_H */
