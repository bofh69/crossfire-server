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

#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextBrowser>

class QHelpEngine;

class HelpBrowser : public QTextBrowser {
    Q_OBJECT
public:
    HelpBrowser(QHelpEngine *helpEngine, QWidget *parent = 0);
    virtual QVariant loadResource(int type, const QUrl &name) override;

private:
    QHelpEngine* helpEngine;
};

#endif /* HELPBROWSER_H */
