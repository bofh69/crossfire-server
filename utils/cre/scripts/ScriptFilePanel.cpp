#include <QtWidgets>
#include "ScriptFilePanel.h"
#include "ScriptFile.h"
#include "ScriptFile.h"
#include "CREMapInformation.h"
#include "CREPixmap.h"

extern "C" {
#include "define.h"
}
#include "assets.h"
#include "AssetsManager.h"

CREScriptPanel::CREScriptPanel(AssetModel *assets, QWidget* parent) : AssetWrapperPanel(parent)
{
    addAssetUseTree(tr("Maps using this script"), assets, "self");
}
