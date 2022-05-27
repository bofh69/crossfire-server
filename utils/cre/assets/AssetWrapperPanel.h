#ifndef ASSETWRAPPERPANEL_H
#define ASSETWRAPPERPANEL_H

#include "CREPanel.h"
#include <QtWidgets>

class AssetWrapper;
class QGridLayout;
class QLabel;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class TreasureListComboBox;
class ArchetypeComboBox;
class AssetUseTree;
class AssetModel;

class AssetWrapperPanel : public CRETPanel<AssetWrapper> {
    Q_OBJECT
public:
    AssetWrapperPanel(QWidget *parent);
    virtual void setItem(AssetWrapper *item) override;

    QLabel *addLabel(const QString &label, const char *property);
    QLineEdit *addLineEdit(const QString &label, const char *property, bool readOnly = true);
    QTextEdit *addTextEdit(const QString &label, const char *property, bool readOnly = true);
    QCheckBox *addCheckBox(const QString &label, const char *property, bool readOnly = true);
    void addFaceChoice(const QString &label, const char *property, bool readOnly = true, bool allowNone = true);
    QSpinBox *addSpinBox(const QString &label, const char *property, int min = 0, int max = 100, bool readOnly = true);
    TreasureListComboBox *addTreasureList(const QString &label, const char *property, bool readOnly = true, bool allowNone = true);
    ArchetypeComboBox *addArchetype(const QString &label, const char *property, bool readOnly = false, bool allowNone = true);
    AssetUseTree *addAssetUseTree(const QString &label, AssetModel *assets, const char *property);
    void addBottomFiller();

protected slots:
    void dataChanged();
    void itemChanged();

protected:
    QGridLayout *myLayout;
    AssetWrapper *myAsset;
    bool myInhibit;
    QMetaObject::Connection myChanged;
    QMetaObject::Connection myDelete;

    template<class T>
    T *addWidget(const QString &label, T *widget, bool sideBySide, const char *property, const char *widgetProperty) {
        int line = myLayout->rowCount();
        if (!label.isEmpty()) {
            myLayout->addWidget(new QLabel(label, this), line, 0, 1, sideBySide ? 1 : 2);
            if (!sideBySide) {
                line++;
            }
        }

        myLayout->addWidget(widget, line, sideBySide ? 1 : 0, 1, sideBySide ? 1 : 2);
        if (property) {
            myLinks.append({property, widget, widgetProperty});
        }
        return widget;
    }

    struct PropertyLink {
        const char *assetPropertyName;
        QWidget *widget;
        const char *widgetPropertyName;
    };
    QList<PropertyLink> myLinks;
};

#endif /* ASSETWRAPPERPANEL_H */
