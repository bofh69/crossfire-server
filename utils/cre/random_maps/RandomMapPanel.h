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
