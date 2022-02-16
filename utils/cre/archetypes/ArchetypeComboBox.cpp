#include "ArchetypeComboBox.h"
#include "CREPixmap.h"
#include "AssetsManager.h"
#include "assets.h"

ArchetypeComboBox::ArchetypeComboBox(QWidget *parent, bool allowNone) : QComboBox(parent) {
    QStringList names;
    if (allowNone) {
        names << "(none)";
        addItem(QIcon(), "(none)", QVariant::fromValue<void*>(nullptr));
    }

    getManager()->archetypes()->each([this, &names] (const archetype *arch) {
        if (arch->head) {
            return;
        }
        QString name = tr("%1 (%2)").arg(arch->clone.name, arch->name);
        addItem(CREPixmap::getIcon(arch->clone.face), name, QVariant::fromValue(static_cast<void *>(const_cast<archetype *>(arch))));
        names << name;
    });

    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setCompleter(new QCompleter(names, this));
    completer()->setFilterMode(Qt::MatchContains);
    completer()->setCaseSensitivity(Qt::CaseInsensitive);
}

void ArchetypeComboBox::setArch(const archetype *arch) {
    setCurrentIndex(findData(QVariant::fromValue(static_cast<void *>(const_cast<archetype *>(arch)))));
}

const archetype *ArchetypeComboBox::arch() const {
    return static_cast<const archetype *>(currentData().value<void *>());
}
