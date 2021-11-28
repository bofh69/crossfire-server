#ifndef RANDOM_MAP_PANEL_H
#define	RANDOM_MAP_PANEL_H

#include "CREPanel.h"

class RandomMap;
class QTextEdit;
class QLabel;

class CRERandomMapPanel : public CRETPanel<const RandomMap>
{
public:
  CRERandomMapPanel(QWidget* parent);
  virtual void setItem(const RandomMap* map) override;
private:
  QLabel* mySource;
  QTextEdit* myInformation;
};

#endif	/* RANDOM_MAP_PANEL_H */
