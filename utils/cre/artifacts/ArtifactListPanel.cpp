#include <QtWidgets>

extern "C" {
#include "global.h"
#include "artifact.h"
}

#include "ArtifactListPanel.h"
#include "CREUtils.h"
#include "animations/AnimationWidget.h"

#include "assets.h"
#include "AssetsManager.h"
#include "Archetypes.h"

ArtifactListPanel::ArtifactListPanel(QWidget *parent) : AssetTWrapperPanel(parent) {
}

void ArtifactListPanel::updateItem() {
}
