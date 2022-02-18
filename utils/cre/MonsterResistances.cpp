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

#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QGraphicsRectItem>

#include "MonsterResistances.h"

extern "C" {
#include "global.h"
#include "attack.h"
}
#include "assets.h"
#include "AssetsManager.h"

class AutoFitView : public QGraphicsView {
public:
    AutoFitView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent) { }

protected:
    virtual void resizeEvent(QResizeEvent *event) override {
        QGraphicsView::resizeEvent(event);
        fitInView(scene()->sceneRect());
    }
};

MonsterResistances::MonsterResistances(QWidget *parent) : QDialog(parent) {
    setModal(true);
    setWindowTitle(tr("Monster resistances"));
    auto layout = new QVBoxLayout(this);

    myScene = new QGraphicsScene(this);

    layout->addWidget(new QLabel(tr("Monster resistances (by steps of 10):")));
    layout->addWidget(myDisplay = new AutoFitView(myScene, this));

    myResistances[ATNR_FIRE] = Qt::red;
    myResistances[ATNR_COLD] = QColor(0, 255, 255);
    myResistances[ATNR_ELECTRICITY] = QColor(0, 0, 255);
    myResistances[ATNR_POISON] = Qt::green;

    buildGraph();
}

#define SCALE       5

void MonsterResistances::buildGraph() {
    std::map<int, std::map<int, int>> values;
    int maxCount = 0, minResist = INT_MAX, maxResist = INT_MIN;

    getManager()->archetypes()->each([&] (const archetype *arch) {
        if (!QUERY_FLAG(&arch->clone, FLAG_MONSTER) || arch->head) {
            return;
        }

        auto mr = arch->clone.resist;
        for (auto r : myResistances) {
            int val = mr[r.first] / 10;
            minResist = qMin(minResist, val);
            maxResist = qMax(maxResist, val);
            values[r.first][val]++;
            maxCount = qMax(values[r.first][val], maxCount);
        }
    });

    myScene->clear();

    int shift = 0;
    for (auto r : myResistances) {
        QPen p(r.second);
        QBrush br(r.second);
        for (int i = minResist; i <= maxResist; i++) {
            if (values[r.first][i] == 0) {
                continue;
            }
            auto item = myScene->addRect(qreal((i - minResist) * SCALE * myResistances.size() + shift), qreal(maxCount - values[r.first][i]), qreal(SCALE - 1), qreal(values[r.first][i]), p, br);
            item->setToolTip(tr("%1% %2: %3 monsters").arg(i * 10).arg(resist_plus[r.first]).arg(values[r.first][i]));
        }
        shift += SCALE;
    }

    myScene->setSceneRect(0, 0, (maxResist - minResist + 1) * SCALE * myResistances.size(), maxCount);
    myDisplay->fitInView(myScene->sceneRect());
}
