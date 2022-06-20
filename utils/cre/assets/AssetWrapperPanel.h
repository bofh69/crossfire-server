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

#ifndef ASSETWRAPPERPANEL_H
#define ASSETWRAPPERPANEL_H

#include <QtWidgets>

#include "AssetWrapper.h"

class TreasureListComboBox;
class ArchetypeComboBox;
class AssetUseTree;
class AssetModel;

/** Base class for a panel displaying information about an asset. */
class AssetWrapperPanel : public QWidget {
    Q_OBJECT
public:
    AssetWrapperPanel(QWidget *parent);
    virtual ~AssetWrapperPanel();

    void addTab(const QString &title);

    virtual void setAsset(AssetWrapper *item);

    QLabel *addLabel(const QString &label, const char *property);
    QLineEdit *addLineEdit(const QString &label, const char *property, bool readOnly = true);
    QTextEdit *addTextEdit(const QString &label, const char *property, bool readOnly = true);
    QCheckBox *addCheckBox(const QString &label, const char *property, bool readOnly = true);
    void addFaceChoice(const QString &label, const char *property, bool readOnly = true, bool allowNone = true);
    void addQuestChoice(const QString &label, const char *property, bool readOnly = true, bool allowNone = true);
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
    QTabWidget *myTab;
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

/**
 * Panel displaying information about a AssetTWrapper.
 */
template<typename T>
class AssetTWrapperPanel : public AssetWrapperPanel {
public:
    AssetTWrapperPanel(QWidget* parent) : AssetWrapperPanel(parent), myItem(nullptr) {};

    virtual void setAsset(AssetWrapper *asset) override {
        AssetWrapperPanel::setAsset(asset);
        auto aw = dynamic_cast<AssetTWrapper<T> *>(asset);
        myItem = aw ? aw->wrappedItem() : nullptr;
        updateItem();
    }
    /** Called when myItem is updated. */
    virtual void updateItem() = 0;

protected:
    T *myItem;  /**< Item to display. */
};

/**
 * Panel displaying information about an AssetWrapper which is its own wrapped item.
 */
template<typename T>
class AssetSWrapperPanel : public AssetWrapperPanel {
public:
    AssetSWrapperPanel(QWidget* parent) : AssetWrapperPanel(parent), myItem(nullptr) {};

    virtual void setAsset(AssetWrapper *asset) override {
        AssetWrapperPanel::setAsset(asset);
        myItem = dynamic_cast<T *>(asset);
        updateItem();
    }
    /** Called when myItem was updated. */
    virtual void updateItem() = 0;

protected:
    T *myItem;  /**< Item to display. */
};

#endif /* ASSETWRAPPERPANEL_H */
