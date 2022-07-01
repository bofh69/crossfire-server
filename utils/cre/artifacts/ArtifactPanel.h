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

#ifndef ARTIFACT_PANEL_H
#define ARTIFACT_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"
#include "../animations/AnimationControl.h"

#include "global.h"
#include "artifact.h"

/**
 * Display details about an artifact definition.
 */
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
