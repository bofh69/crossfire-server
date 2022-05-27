#ifndef SCRIPT_FILE_PANEL_H
#define SCRIPT_FILE_PANEL_H

#include "assets/AssetWrapperPanel.h"

class AssetModel;

class CREScriptPanel : public AssetWrapperPanel {
public:
    CREScriptPanel(AssetModel *assets, QWidget *parent);
};

#endif /* SCRIPT_FILE_PANEL_H */
