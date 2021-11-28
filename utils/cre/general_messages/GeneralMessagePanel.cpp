#include "GeneralMessagePanel.h"

CREGeneralMessagePanel::CREGeneralMessagePanel(QWidget* parent) : AssetWrapperPanel(parent) {
    addLineEdit(tr("Title:"), "title");
    addLineEdit(tr("Identifier:"), "identifier");
    addLineEdit(tr("Quest:"), "quest");
    addLineEdit(tr("Chance:"), "chance");
    addFaceChoice(tr("Face:"), "face");
    addTextEdit(tr("Message:"), "message");
}
