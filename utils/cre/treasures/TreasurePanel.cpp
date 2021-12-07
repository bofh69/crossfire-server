#include "TreasurePanel.h"
#include <QMimeData>
#include "assets.h"
#include "AssetsManager.h"
#include "archetypes/ArchetypeComboBox.h"
#include "treasures/TreasureListComboBox.h"

TreasurePanel::TreasurePanel(QWidget* parent) : AssetWrapperPanel(parent) {
    addSpinBox(tr("Chance:"), "chance", 0, 255, false);
    addSpinBox(tr("Magic:"), "magic", 0, 255, false);
    addSpinBox(tr("Count:"), "nrof", 0, 65535, false);
    myList = addTreasureList(tr("Treasure:"), "list", false);
    myArch = addArchetype(tr("Archetype:"), "arch");
    addBottomFiller();
    setAcceptDrops(true);
}

void TreasurePanel::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("x-crossfire/archetype") || event->mimeData()->hasFormat("x-crossfire/treasure-list")) {
        event->acceptProposedAction();
    }
}

void TreasurePanel::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void TreasurePanel::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("x-crossfire/archetype")) {
        QByteArray ba(event->mimeData()->data("x-crossfire/archetype"));
        QDataStream df(&ba, QIODevice::ReadOnly);
        QString name;
        while (!df.atEnd()) {
            df >> name;
        }
        if (!name.isEmpty()) {
            myArch->setArch(getManager()->archetypes()->find(name.toLocal8Bit().data()));
        }
        event->acceptProposedAction();
    }
    if (event->mimeData()->hasFormat("x-crossfire/treasure-list")) {
        QByteArray ba(event->mimeData()->data("x-crossfire/treasure-list"));
        QDataStream df(&ba, QIODevice::ReadOnly);
        QString name;
        while (!df.atEnd()) {
            df >> name;
        }
        if (!name.isEmpty()) {
            myList->setList(getManager()->treasures()->find(name.toLocal8Bit().data()));
        }
        event->acceptProposedAction();
    }
}
