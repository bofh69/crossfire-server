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

#ifndef MONSTERR_ESISTANCES_H
#define MONSTERR_ESISTANCES_H

#include <QDialog>
#include <map>
class QGraphicsView;
class QGraphicsScene;

/**
 * Display a histogram of monster resistances to fire, cold, electricity, poison.
 */
class MonsterResistances : public QDialog {
    Q_OBJECT
public:
  MonsterResistances(QWidget *parent);

private:
  void buildGraph();

  QGraphicsView *myDisplay;
  QGraphicsScene *myScene;
  std::map<int, QColor> myResistances;
};

#endif /* MONSTER_RESISTANCES_H */
