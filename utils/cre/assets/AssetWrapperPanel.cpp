#include "AssetWrapperPanel.h"
#include "AssetWrapper.h"
#include <QVariant>
#include <QGridLayout>
#include "FaceComboBox.h"
#include "treasures/TreasureListComboBox.h"
#include "archetypes/ArchetypeComboBox.h"

AssetWrapperPanel::AssetWrapperPanel(QWidget *parent) : CRETPanel(parent), myAsset(nullptr), myInhibit(false) {
    myLayout = new QGridLayout(this);
}

void AssetWrapperPanel::setItem(AssetWrapper *item) {
    if (myAsset) {
        disconnect(myChanged);
        disconnect(myDelete);
    }
    myAsset = item;
    if (myAsset) {
        myChanged = connect(myAsset, SIGNAL(modified()), this, SLOT(itemChanged()));
        myDelete = connect(myAsset, &QObject::destroyed, [this] () { setItem(nullptr); });
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
    return addWidget(label, new QLabel(this), true, property, "text");
}

void AssetWrapperPanel::addLineEdit(const QString &label, const char *property, bool readOnly) {
    auto widget = addWidget(label, new QLineEdit(this), true, property, "text");
    if (readOnly) {
        widget->setReadOnly(readOnly);
    } else {
        connect(widget, SIGNAL(editingFinished()), this, SLOT(dataChanged()));
    }
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
