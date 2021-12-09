#include "GeneralMessagePanel.h"

CREGeneralMessagePanel::CREGeneralMessagePanel(QWidget* parent) : AssetWrapperPanel(parent) {
    addLineEdit(tr("Title:"), "title", false);
    addLineEdit(tr("Identifier:"), "identifier");
    addLineEdit(tr("Quest:"), "quest", false);
    addLineEdit(tr("Chance:"), "chance", false);
    addFaceChoice(tr("Face:"), "face", false);
    addTextEdit(tr("Message:"), "message", false);
}
