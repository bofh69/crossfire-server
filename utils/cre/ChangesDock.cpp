#include "ChangesDock.h"
#include <QTextEdit>
#include <QFile>
#include "CRESettings.h"

ChangesDock::ChangesDock(QWidget *parent) : QDockWidget(tr("Changes"), parent) {
    setAllowedAreas(Qt::RightDockWidgetArea);
    setFeatures(DockWidgetClosable);

    QTextEdit *myText = new QTextEdit(this);
    myText->setReadOnly(true);
    setWidget(myText);

    QFile changes(":/resources/changes.html");
    changes.open(QIODevice::ReadOnly);
    QString content = changes.readAll();

    CRESettings settings;
    setVisible(false);
    if (settings.showChanges() && settings.changesLength() != content.length()) {
        setVisible(true);
        settings.setChangesLength(content.length());
    }

    myText->setText(content);
}
