#ifndef SCRIPT_FILE_PANEL_H
#define SCRIPT_FILE_PANEL_H

#include <QWidget>
#include "CREPanel.h"

class ScriptFile;

class CREScriptPanel : public CRETPanel<ScriptFile> {
public:
    CREScriptPanel(QWidget *parent);

    virtual void setItem(ScriptFile *script) override;

private:
    QTreeWidget* myMaps;
};

#endif /* SCRIPT_FILE_PANEL_H */
