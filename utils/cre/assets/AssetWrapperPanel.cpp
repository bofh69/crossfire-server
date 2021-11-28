#include "AssetWrapperPanel.h"
#include "AssetWrapper.h"
#include <QVariant>
#include <QGridLayout>
#include "FaceComboBox.h"

AssetWrapperPanel::AssetWrapperPanel(QWidget *parent) : CRETPanel(parent) {
    myLayout = new QGridLayout(this);
}

void AssetWrapperPanel::setItem(AssetWrapper *item) {
    for (auto pl : myLinks) {
        pl.widget->setProperty(pl.widgetPropertyName, item->property(pl.assetPropertyName));
    }
}

QLabel *AssetWrapperPanel::addLabel(const QString &label, const char *property) {
    return addWidget(label, new QLabel(this), true, property, "text");
}

void AssetWrapperPanel::addLineEdit(const QString &label, const char *property, bool readOnly) {
    auto widget = addWidget(label, new QLineEdit(this), true, property, "text");
    widget->setReadOnly(readOnly);
}

QTextEdit *AssetWrapperPanel::addTextEdit(const QString &label, const char *property, bool readOnly) {
    auto widget = addWidget(label, new QTextEdit(this), false, property, "plainText");
    widget->setReadOnly(readOnly);
    return widget;
}

void AssetWrapperPanel::addCheckBox(const QString &label, const char *property, bool readOnly) {
    auto widget = addWidget(label, new QCheckBox(this), true, property, "checked");
    widget->setEnabled(!readOnly);
}

void AssetWrapperPanel::addFaceChoice(const QString &label, const char *property, bool readOnly, bool allowNone) {
    auto widget = addWidget(label, new FaceComboBox(this, allowNone), true, property, "face");
    widget->setEnabled(!readOnly);
}

void AssetWrapperPanel::addBottomFiller() {
    QWidget *bottomFiller = new QWidget(this);
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    myLayout->addWidget(bottomFiller, myLayout->rowCount(), 0, 1, 2);
}
