#ifndef FACESETS_PANEL_H
#define FACESETS_PANEL_H

#include <QWidget>
#include "assets/AssetWrapperPanel.h"
class QLabel;
class LicenseManager;

class FacesetsPanel : public AssetWrapperPanel {
public:
    FacesetsPanel(QWidget* parent, LicenseManager *licenseManager);

    virtual void setItem(AssetWrapper *asset) override;

protected:
    LicenseManager *myLicenseManager;
    QLabel* myImages;
    QLabel* myLicenses;
};

#endif /* FACESETS_PANEL_H */
