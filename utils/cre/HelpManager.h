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

#ifndef HELP_MANAGER_H
#define HELP_MANAGER_H

#include <QHelpEngine>

class QDialog;
class HelpBrowser;
class AssetWrapper;
class QTreeView;

class HelpManager : public QHelpEngine {
    Q_OBJECT
public:
    static const char *assetTreeProperty;
    static const char *helpIdProperty;

    HelpManager(const QString &helpRoot);
    virtual ~HelpManager();

    static void setHelpId(QWidget *widget, const QString &id);

public slots:
    void displayHelp();

protected:
    QUrl computeUrlForWidget(QWidget *widget) const;
    QUrl computeUrlToDisplay() const;

    QDialog *myDisplay;
    HelpBrowser *myBrowser;
};

#endif /* HELP_MANAGER_H */
