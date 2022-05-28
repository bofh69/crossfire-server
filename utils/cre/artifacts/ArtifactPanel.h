#ifndef ARTIFACT_PANEL_H
#define ARTIFACT_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"
#include "../animations/AnimationControl.h"

extern "C" {
#include "global.h"
#include "artifact.h"
}

class ArtifactPanel : public AssetTWrapperPanel<artifact>
{
    Q_OBJECT

    public:
        ArtifactPanel(QWidget* parent);
        virtual void updateItem() override;

    protected:
        QLineEdit* myName;
        QLineEdit* myChance;
        QLineEdit* myType;
        QLabel* myViaAlchemy;
        QTreeWidget* myArchetypes;
        QTextEdit* myValues;
        QTextEdit* myInstance;
        AnimationControl* myAnimation;
        AnimationWidget* myFace;

        void computeMadeViaAlchemy(const artifact* artifact) const;

    protected slots:
        void artifactChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
};

#endif // ARTIFACT_PANEL_H
