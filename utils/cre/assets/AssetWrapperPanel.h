#ifndef ASSETWRAPPERPANEL_H
#define ASSETWRAPPERPANEL_H

#include "CREPanel.h"
#include <QtWidgets>

class AssetWrapper;
class QGridLayout;
class QLabel;
class QLineEdit;

class AssetWrapperPanel : public CRETPanel<AssetWrapper> {
public:
    AssetWrapperPanel(QWidget *parent);
    virtual void setItem(AssetWrapper *item) override;

    QLabel *addLabel(const QString &label, const char *property);
    void addLineEdit(const QString &label, const char *property, bool readOnly = true);
    QTextEdit *addTextEdit(const QString &label, const char *property, bool readOnly = true);
    void addCheckBox(const QString &label, const char *property, bool readOnly = true);
    void addFaceChoice(const QString &label, const char *property, bool readOnly = true, bool allowNone = true);
    void addBottomFiller();

protected:
    QGridLayout *myLayout;

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
