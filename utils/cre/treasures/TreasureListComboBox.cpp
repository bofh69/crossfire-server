#include "TreasureListComboBox.h"
#include "CREPixmap.h"
#include "AssetsManager.h"
#include "assets.h"

TreasureListComboBox::TreasureListComboBox(QWidget *parent, bool allowNone) : QComboBox(parent) {
    QStringList names;
    if (allowNone) {
        names << "(none)";
        addItem(QIcon(), "(none)", QVariant::fromValue<void*>(nullptr));
    }

    getManager()->treasures()->each([this, &names] (const treasurelist *list) {
        addItem(list->total_chance == 0 ? CREPixmap::getTreasureIcon() : CREPixmap::getTreasureOneIcon(), list->name, QVariant::fromValue(static_cast<void *>(const_cast<treasurelist *>(list))));
        names << list->name;
    });

    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setCompleter(new QCompleter(names, this));
    completer()->setFilterMode(Qt::MatchContains);
    completer()->setCaseSensitivity(Qt::CaseInsensitive);
}

void TreasureListComboBox::setList(const treasurelist *list) {
    setCurrentIndex(findData(QVariant::fromValue(static_cast<void *>(const_cast<treasurelist *>(list)))));
}

const treasurelist *TreasureListComboBox::list() const {
    return static_cast<const treasurelist *>(currentData().value<void *>());
}
