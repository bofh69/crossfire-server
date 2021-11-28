#include <QtWidgets>

#include "FacesetsPanel.h"

#include "assets.h"
#include "AssetsManager.h"
#include "LicenseManager.h"
#include "FacesetsWrapper.h"

FacesetsPanel::FacesetsPanel(QWidget* parent) : AssetWrapperPanel(parent) {
    addLabel(tr("Prefix:"), "prefix");
    addLabel(tr("Full name:"), "fullname");
    addLabel(tr("Fallback:"), "fallback");
    addLabel(tr("Size:"), "size");
    addLabel(tr("Extension:"), "extension");
    addWidget(tr("Defined faces:"), myImages = new QLabel(this), true, nullptr, nullptr);
    addWidget(tr("License information:"), myLicenses = new QLabel(this), true, nullptr, nullptr);
    addBottomFiller();
}

void FacesetsPanel::setItem(AssetWrapper *asset) {
    AssetWrapperPanel::setItem(asset);
    auto fs = dynamic_cast<FacesetsWrapper *>(asset)->item();

    size_t count = 0, total = getManager()->faces()->count(), licenses = 0;

    getManager()->faces()->each([&count, &licenses, &fs] (const Face * face) {
        if (face->number < fs->allocated && fs->faces[face->number].datalen > 0) {
            count++;
            if (LicenseManager::get()->getForFace(face->name).count(fs->prefix) > 0) {
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
