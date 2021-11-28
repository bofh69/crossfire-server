#ifndef FACESETS_PANEL_H
#define FACESETS_PANEL_H

#include <QWidget>
#include "assets/AssetWrapperPanel.h"
class QLabel;

class FacesetsPanel : public AssetWrapperPanel {
public:
    FacesetsPanel(QWidget* parent);

    virtual void setItem(AssetWrapper *asset) override;

protected:
    QLabel* myImages;
    QLabel* myLicenses;
};

#endif /* FACESETS_PANEL_H */
