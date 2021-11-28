#include <Qt>

#include "FacePanel.h"
#include "CREUtils.h"
#include "CRESmoothFaceMaker.h"
#include "assets.h"
#include "AssetsManager.h"
#include "LicenseManager.h"
#include "CREMapInformationManager.h"
#include "Quests.h"
#include "assets/AssetModel.h"
#include "ResourcesManager.h"

/** @todo duplication with common/image */
static const char *const colorname[] = {
    "black",                    /* 0  */
    "white",                    /* 1  */
    "blue",                     /* 2  */
    "red",                      /* 3  */
    "orange",                   /* 4  */
    "light_blue",               /* 5  */
    "dark_orange",              /* 6  */
    "green",                    /* 7  */
    "light_green",              /* 8  */
    "grey",                     /* 9  */
    "brown",                    /* 10 */
    "yellow",                   /* 11 */
    "khaki"                     /* 12 */
};


CREFacePanel::CREFacePanel(QWidget* parent, AssetModel *model, ResourcesManager *resources, CREMapInformationManager* maps) : CRETPanel(parent) {
    myMaps = maps;
    myFace = 0;
    myResources = resources;

    QGridLayout* layout = new QGridLayout(this);

    myUseModel = new UseFilterAssetModel(this);
    myUseModel->setSourceModel(model);

    myUseView = new QTreeView(this);
    myUseView->setIconSize(QSize(32, 32));
    myUseView->setModel(myUseModel);
    layout->addWidget(myUseView, 1, 1, 3, 2);

    myLicenses = new QTreeWidget(this);
    myLicenses->setColumnCount(2);
    myLicenses->setHeaderLabels(QStringList(tr("License field")) << "Value");
    myLicenses->setIconSize(QSize(32, 32));
    myLicenses->setRootIsDecorated(false);
    layout->addWidget(myLicenses, 1, 3, 3, 2);

    myColor = new QComboBox(this);
    layout->addWidget(new QLabel("Magicmap color: "), 4, 1);
    layout->addWidget(myColor, 4, 2, 1, 3);

    for(uint color = 0; color < sizeof(colorname) / sizeof(*colorname); color++)
        myColor->addItem(colorname[color], color);

    myFile = new QLineEdit(this);
    myFile->setReadOnly(true);
    layout->addWidget(new QLabel("Original file: "), 5, 1);
    layout->addWidget(myFile, 5, 2, 1, 3);

    mySave = new QPushButton(tr("Save face"));
    layout->addWidget(mySave, 6, 1);
    connect(mySave, SIGNAL(clicked(bool)), this, SLOT(saveClicked(bool)));

    QPushButton* smooth = new QPushButton(tr("Make smooth base"), this);
    layout->addWidget(smooth, 6, 2, 1, 3);
    connect(smooth, SIGNAL(clicked(bool)), this, SLOT(makeSmooth(bool)));
}

void CREFacePanel::setItem(Face* face)
{
    Q_ASSERT(face);
    myFace = face;

    myUseModel->setFilter(myResources->wrap(face, nullptr));
    myUseView->expandAll();

    myColor->setCurrentIndex(myFace->magicmap);

    myLicenses->clear();

    auto licenses = LicenseManager::get()->getForFace(myFace->name);
    for (auto l : licenses) {
        QTreeWidgetItem *wi = new QTreeWidgetItem(QStringList(QString(l.first.c_str())));
        for (auto p : l.second) {
            auto twi = new QTreeWidgetItem(wi, QStringList(p.first.c_str()) << p.second.c_str());
            twi->setToolTip(1, p.second.c_str());
        }
        myLicenses->addTopLevelItem(wi);
        wi->setExpanded(true);
    }

}
void CREFacePanel::saveClicked(bool)
{
}

void CREFacePanel::makeSmooth(bool)
{
    CRESmoothFaceMaker maker;
    maker.setSelectedFace(myFace);
    maker.setAutoClose();
    maker.exec();
}
