#ifndef ARTIFACT_LIST_PANEL_H
#define ARTIFACT_LIST_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"

extern "C" {
#include "global.h"
#include "artifact.h"
}

class ArtifactListPanel : public AssetTWrapperPanel<const artifactlist> {
Q_OBJECT

public:
    ArtifactListPanel(QWidget* parent);
    virtual void updateItem() override;
};

#endif // ARTIFACT_LIST_PANEL_H
