#include "HelpBrowser.h"
#include <QHelpEngine>

HelpBrowser::HelpBrowser(QHelpEngine* helpEngine, QWidget* parent) : QTextBrowser(parent), helpEngine(helpEngine) {
}

QVariant HelpBrowser::loadResource(int, const QUrl &name) {
    if (name.scheme() == "qthelp") {
        return QVariant(helpEngine->fileData(name));
    }
    return QVariant();
}
