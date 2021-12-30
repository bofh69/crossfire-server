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
