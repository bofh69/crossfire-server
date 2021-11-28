#include <QtWidgets>

#include "RegionPanel.h"

RegionPanel::RegionPanel(QWidget* parent) : AssetWrapperPanel(parent) {
    addLabel(tr("Short name:"), "shortName");
    addLabel(tr("Long name:"), "longName");
    addLabel(tr("Message:"), "message")->setWordWrap(true);
    addLabel(tr("Jail:"), "jailPath");
    addLabel("", "jailX");
    addLabel("", "jailY");
    addBottomFiller();
}
