#include "TreasurePanel.h"
#include <QMimeData>
#include "assets.h"
#include "AssetsManager.h"
#include "archetypes/ArchetypeComboBox.h"
#include "treasures/TreasureListComboBox.h"
#include "MimeUtils.h"
#include "HelpManager.h"

TreasurePanel::TreasurePanel(QWidget* parent) : AssetWrapperPanel(parent) {
    addSpinBox(tr("Chance:"), "chance", 0, 255, false);
    addSpinBox(tr("Magic:"), "magic", 0, 255, false);
    addSpinBox(tr("Count:"), "nrof", 0, 65535, false);
    myList = addTreasureList(tr("Treasure:"), "list", false);
    myArch = addArchetype(tr("Archetype:"), "arch");
    addBottomFiller();
    setAcceptDrops(true);
    HelpManager::setHelpId(this, "treasures");
}

void TreasurePanel::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat(MimeUtils::Archetype) || event->mimeData()->hasFormat(MimeUtils::TreasureList)) {
        event->acceptProposedAction();
    }
}

void TreasurePanel::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void TreasurePanel::dropEvent(QDropEvent *event) {
    auto archs = MimeUtils::extract(event->mimeData(), MimeUtils::Archetype, getManager()->archetypes());
    if (!archs.empty()) {
        myArch->setArch(archs.front());
        event->acceptProposedAction();
    }
    auto lists = MimeUtils::extract(event->mimeData(), MimeUtils::TreasureList, getManager()->treasures());
    if (!lists.empty()) {
        myList->setList(lists.front());
        event->acceptProposedAction();
    }
}
