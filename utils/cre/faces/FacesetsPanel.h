#ifndef FACESETS_PANEL_H
#define FACESETS_PANEL_H

#include <QWidget>
#include "assets/AssetWrapperPanel.h"

extern "C" {
#include "global.h"
#include "image.h"
}

class QLabel;
class LicenseManager;

class FacesetsPanel : public AssetTWrapperPanel<face_sets> {
public:
    FacesetsPanel(QWidget* parent, LicenseManager *licenseManager);

    virtual void updateItem() override;

protected:
    LicenseManager *myLicenseManager;
    QLabel* myImages;
    QLabel* myLicenses;
};

#endif /* FACESETS_PANEL_H */
