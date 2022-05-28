#ifndef FACE_PANEL_H
#define FACE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"

extern "C" {
#include "global.h"
}

class CREMapInformationManager;
class AssetModel;
class UseFilterAssetModel;
class ResourcesManager;

class CREFacePanel : public AssetTWrapperPanel<Face> {
    Q_OBJECT

public:
    CREFacePanel(QWidget* parent, AssetModel *model, ResourcesManager *resources, CREMapInformationManager* maps);
    virtual void updateItem() override;

protected:
    CREMapInformationManager* myMaps;

    QTreeView *myUseView;
    UseFilterAssetModel *myUseModel;
    ResourcesManager *myResources;
    QComboBox* myColor;
    QLineEdit* myFile;
    QPushButton* mySave;
    QTreeWidget* myLicenses;

private slots:
    void saveClicked(bool);
    void makeSmooth(bool);
};

#endif // FACE_PANEL_H
