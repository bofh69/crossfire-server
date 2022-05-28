/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "AssetWrapperPanel.h"
#include "AssetWrapper.h"
#include <QVariant>
#include <QGridLayout>
#include "FaceComboBox.h"
#include "treasures/TreasureListComboBox.h"
#include "archetypes/ArchetypeComboBox.h"
#include "assets/AssetUseTree.h"

AssetWrapperPanel::AssetWrapperPanel(QWidget *parent) : QWidget(parent), myAsset(nullptr), myInhibit(false) {
    myLayout = new QGridLayout(this);
}

AssetWrapperPanel::~AssetWrapperPanel() {
    if (myChanged) {
        disconnect(myChanged);
    }
    if (myDelete) {
        disconnect(myDelete);
    }
}
void AssetWrapperPanel::setAsset(AssetWrapper *asset) {
    if (myAsset) {
        disconnect(myChanged);
        disconnect(myDelete);
    }
    myAsset = asset;
    if (myAsset) {
        myChanged = connect(myAsset, SIGNAL(modified()), this, SLOT(itemChanged()));
        myDelete = connect(myAsset, &QObject::destroyed, [this] () { setAsset(nullptr); });
        itemChanged();
    }
}

void AssetWrapperPanel::itemChanged() {
    if (!myAsset || myInhibit) {
        return;
    }
    for (auto pl : myLinks) {
        pl.widget->setProperty(pl.widgetPropertyName, myAsset->property(pl.assetPropertyName));
    }
}

QLabel *AssetWrapperPanel::addLabel(const QString &label, const char *property) {
    return addWidget(label, new QLabel(this), !label.isEmpty(), property, "text");
}

QLineEdit *AssetWrapperPanel::addLineEdit(const QString &label, const char *property, bool readOnly) {
    auto widget = addWidget(label, new QLineEdit(this), true, property, "text");
    if (readOnly) {
        widget->setReadOnly(readOnly);
    } else {
        connect(widget, SIGNAL(editingFinished()), this, SLOT(dataChanged()));
    }
    return widget;
}

QTextEdit *AssetWrapperPanel::addTextEdit(const QString &label, const char *property, bool readOnly) {
    auto widget = addWidget(label, new QTextEdit(this), false, property, "plainText");
    if (readOnly) {
        widget->setReadOnly(readOnly);
    } else {
        connect(widget, SIGNAL(textChanged()), this, SLOT(dataChanged()));
    }
    return widget;
}

QCheckBox *AssetWrapperPanel::addCheckBox(const QString &label, const char *property, bool readOnly) {
    auto widget = addWidget(label, new QCheckBox(this), true, property, "checked");
    if (readOnly) {
        widget->setEnabled(false);
    } else {
        connect(widget, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
    }
    return widget;
}

void AssetWrapperPanel::addFaceChoice(const QString &label, const char *property, bool readOnly, bool allowNone) {
    auto widget = addWidget(label, new FaceComboBox(this, allowNone), true, property, "face");
    widget->setEnabled(!readOnly);
}

QSpinBox *AssetWrapperPanel::addSpinBox(const QString &label, const char *property, int min, int max, bool readOnly) {
    auto widget = addWidget(label, new QSpinBox(this), true, property, "value");
    widget->setMinimum(min);
    widget->setMaximum(max);
    if (readOnly) {
        widget->setReadOnly(true);
    } else {
        connect(widget, SIGNAL(valueChanged(int)), this, SLOT(dataChanged()));
    }
    return widget;
}

TreasureListComboBox *AssetWrapperPanel::addTreasureList(const QString &label, const char *property, bool readOnly, bool allowNone) {
    auto widget = addWidget(label, new TreasureListComboBox(this, allowNone), true, property, "list");
    if (readOnly) {
        widget->setEnabled(false);
    } else {
        connect(widget, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
    }
    return widget;
}

ArchetypeComboBox *AssetWrapperPanel::addArchetype(const QString &label, const char *property, bool readOnly, bool allowNone) {
    auto widget = addWidget(label, new ArchetypeComboBox(this, allowNone), true, property, "arch");
    if (readOnly) {
        widget->setEnabled(false);
    } else {
        connect(widget, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
    }
    return widget;
}

AssetUseTree *AssetWrapperPanel::addAssetUseTree(const QString &label, AssetModel *assets, const char *property) {
    return addWidget(label, new AssetUseTree(assets, this), false, property, "filter");
}

void AssetWrapperPanel::addBottomFiller() {
    QWidget *bottomFiller = new QWidget(this);
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    myLayout->addWidget(bottomFiller, myLayout->rowCount(), 0, 1, 2);
}

void AssetWrapperPanel::dataChanged() {
    if (!myAsset) {
        return;
    }
    QObject *widget = sender();
    for (auto link : myLinks) {
        if (link.widget == widget) {
            myInhibit = true;
            myAsset->setProperty(link.assetPropertyName, widget->property(link.widgetPropertyName));
            myInhibit = false;
            break;
        }
    }
}
