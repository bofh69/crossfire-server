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

#ifndef FACESETS_PANEL_H
#define FACESETS_PANEL_H

#include <QWidget>
#include "assets/AssetWrapperPanel.h"

#include "global.h"
#include "image.h"

class QLabel;
class LicenseManager;

/**
 * Display details about a faceset.
 */
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
