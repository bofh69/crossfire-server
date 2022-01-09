#ifndef CHANGES_DOCK_H
#define CHANGES_DOCK_H

#include <QDockWidget>

class QHelpEngineCore;
class QTextEdit;

class ChangesDock : public QDockWidget {
public:
    ChangesDock(QHelpEngineCore *help, QWidget *parent);

protected:
    void helpReady(QHelpEngineCore *help, QTextEdit *edit);
};

#endif /* CHANGES_DOCK_H */
