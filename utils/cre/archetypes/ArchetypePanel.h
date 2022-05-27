#ifndef ARCHETYPE_PANEL_H
#define ARCHETYPE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "../assets/AssetWrapperPanel.h"

extern "C" {
#include "global.h"
}

class CREMapInformationManager;
class ResourcesManager;
class AssetModel;

class ArchetypePanel : public AssetWrapperPanel {
    Q_OBJECT

public:
    ArchetypePanel(CREMapInformationManager* store, ResourcesManager* resources, AssetModel* model, QWidget* parent);
    virtual void setItem(AssetWrapper* asset) override;

    virtual void commitData() override;

protected:
    CREMapInformationManager* myStore;
    ResourcesManager *myResources;
    QTextEdit* myDisplay;
    std::string myInitialArch;
    archt* myArchetype;
};

#endif // ARCHETYPE_PANEL_H
