#include "ChangesDock.h"
#include <QTextEdit>
#include <QFile>
#include "CRESettings.h"
#include <QHelpEngineCore>

ChangesDock::ChangesDock(QHelpEngineCore *help, QWidget *parent) : QDockWidget(tr("Changes"), parent) {
    setAllowedAreas(Qt::RightDockWidgetArea);
    setFeatures(DockWidgetClosable);
    setVisible(false);

    QTextEdit *changes = new QTextEdit(this);
    changes->setReadOnly(true);
    setWidget(changes);

    connect(help, &QHelpEngineCore::setupFinished, [this, help, changes] () { helpReady(help, changes); });
}

void ChangesDock::helpReady(QHelpEngineCore *help, QTextEdit *edit) {
    QString content("No content to display");
    auto links = help->linksForIdentifier("changes");
    if (!links.empty()) {
        content = help->fileData(links.begin().value());
    }
    edit->setText(content);

    CRESettings settings;
    if (settings.showChanges() && settings.changesLength() != content.length()) {
        setVisible(true);
        settings.setChangesLength(content.length());
    }

}
