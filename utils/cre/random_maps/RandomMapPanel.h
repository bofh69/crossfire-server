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

#ifndef RANDOM_MAP_PANEL_H
#define	RANDOM_MAP_PANEL_H

#include "assets/AssetWrapperPanel.h"

class RandomMap;
class QTextEdit;
class QLabel;

class CRERandomMapPanel : public AssetSWrapperPanel<const RandomMap>
{
public:
  CRERandomMapPanel(QWidget* parent);
  virtual void updateItem() override;
private:
  QLabel* mySource;
  QTextEdit* myInformation;
};

#endif	/* RANDOM_MAP_PANEL_H */
