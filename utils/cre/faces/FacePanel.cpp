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

#include "FacePanel.h"
#include "CRESmoothFaceMaker.h"
#include "LicenseManager.h"
#include "assets/AssetModel.h"
#include "ResourcesManager.h"

CREFacePanel::CREFacePanel(QWidget* parent, AssetModel *model, ResourcesManager *resources, CREMapInformationManager* maps) : AssetTWrapperPanel(parent) {
    myMaps = maps;
    myResources = resources;

    myUseModel = new UseFilterAssetModel(this);
    myUseModel->setSourceModel(model);

    myUseView = new QTreeView(this);
    myUseView->setIconSize(QSize(32, 32));
    myUseView->setModel(myUseModel);
    myLayout->addWidget(myUseView, 1, 1, 3, 2);

    myLicenses = new QTreeWidget(this);
    myLicenses->setColumnCount(2);
    myLicenses->setHeaderLabels(QStringList(tr("License field")) << tr("Value"));
    myLicenses->setIconSize(QSize(32, 32));
    myLicenses->setRootIsDecorated(false);
    myLayout->addWidget(myLicenses, 1, 3, 3, 2);

    myColor = new QComboBox(this);
    myLayout->addWidget(new QLabel(tr("Magicmap color:")), 4, 1);
    myLayout->addWidget(myColor, 4, 2, 1, 3);

    const char *name;
    for(uint color = 0; strcmp(name = get_colorname(color), "") != 0; color++)
        myColor->addItem(name, color);

    myFile = new QLineEdit(this);
    myFile->setReadOnly(true);
    myLayout->addWidget(new QLabel(tr("Face file:")), 5, 1);
    myLayout->addWidget(myFile, 5, 2, 1, 3);

    mySave = new QPushButton(tr("Save face"));
    myLayout->addWidget(mySave, 6, 1);
    connect(mySave, SIGNAL(clicked(bool)), this, SLOT(saveClicked(bool)));

    QPushButton* smooth = new QPushButton(tr("Make smooth base"), this);
    myLayout->addWidget(smooth, 6, 2, 1, 3);
    connect(smooth, SIGNAL(clicked(bool)), this, SLOT(makeSmooth(bool)));
}

void CREFacePanel::updateItem()
{
    myUseModel->setFilter(myAsset);
    myUseView->expandAll();

    myColor->setCurrentIndex(myItem->magicmap);

    myLicenses->clear();

    auto licenses = myResources->licenseManager()->getForFace(myItem->name);
    for (auto l : licenses) {
        QTreeWidgetItem *wi = new QTreeWidgetItem(QStringList(QString(l.first.c_str())));
        for (auto p : l.second) {
            auto twi = new QTreeWidgetItem(wi, QStringList(p.first.c_str()) << p.second.c_str());
            twi->setToolTip(1, p.second.c_str());
        }
        myLicenses->addTopLevelItem(wi);
        wi->setExpanded(true);
    }

    myFile->setText(QString::fromStdString(myResources->originOf(myItem)));
}
void CREFacePanel::saveClicked(bool)
{
}

void CREFacePanel::makeSmooth(bool)
{
    CRESmoothFaceMaker maker;
    maker.setSelectedFace(myItem);
    maker.setAutoClose();
    maker.exec();
}
