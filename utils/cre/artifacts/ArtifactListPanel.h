#ifndef ARTIFACT_LIST_PANEL_H
#define ARTIFACT_LIST_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "CREPanel.h"

extern "C" {
#include "global.h"
#include "artifact.h"
}

class ArtifactListPanel : public CRETPanel<const artifactlist> {
Q_OBJECT

public:
    ArtifactListPanel(QWidget* parent);
    virtual void setItem(const artifactlist* al) override;
};

#endif // ARTIFACT_LIST_PANEL_H
