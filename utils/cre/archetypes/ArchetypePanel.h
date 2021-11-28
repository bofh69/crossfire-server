#ifndef ARCHETYPE_PANEL_H
#define ARCHETYPE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "CREPanel.h"

extern "C" {
#include "global.h"
}

class CREMapInformationManager;
class ResourcesManager;

class ArchetypePanel : public CRETPanel<archt> {
    Q_OBJECT

public:
    ArchetypePanel(CREMapInformationManager* store, ResourcesManager* resources, QWidget* parent);
    virtual void setItem(archt* archetype) override;

    virtual void commitData() override;

protected:
    CREMapInformationManager* myStore;
    ResourcesManager *myResources;
    QTextEdit* myDisplay;
    std::string myInitialArch;
    QTreeWidget* myUsing;
    archt* myArchetype;
};

#endif // ARCHETYPE_PANEL_H
