#include <QtWidgets>

#include "FacesetsPanel.h"

#include "assets.h"
#include "AssetsManager.h"
#include "LicenseManager.h"
#include "FacesetsWrapper.h"

FacesetsPanel::FacesetsPanel(QWidget* parent, LicenseManager *licenseManager) : AssetTWrapperPanel(parent), myLicenseManager(licenseManager) {
    addLabel(tr("Prefix:"), "prefix");
    addLabel(tr("Full name:"), "fullname");
    addLabel(tr("Fallback:"), "fallback");
    addLabel(tr("Size:"), "size");
    addLabel(tr("Extension:"), "extension");
    addWidget(tr("Defined faces:"), myImages = new QLabel(this), true, nullptr, nullptr);
    addWidget(tr("License information:"), myLicenses = new QLabel(this), true, nullptr, nullptr);
    addBottomFiller();
}

void FacesetsPanel::updateItem() {
    size_t count = 0, total = getManager()->faces()->count(), licenses = 0;

    getManager()->faces()->each([&] (const Face * face) {
        if (face->number < myItem->allocated && myItem->faces[face->number].datalen > 0) {
            count++;
            if (myLicenseManager->getForFace(face->name).count(myItem->prefix) > 0) {
                licenses++;
            }
        }
    });

    uint8_t percent = count * 100 / total;
    if (percent == 100 && count < total)
        percent = 99;

    myImages->setText(QString("%1 out of %2 (%3%)").arg(count).arg(total).arg(percent));

    percent = licenses * 100 / count;
    if (percent == 100 && licenses < count)
        percent = 99;
    myLicenses->setText(QString("%1 faces have license information out of %2 (%3%)").arg(licenses).arg(count).arg(percent));
}
