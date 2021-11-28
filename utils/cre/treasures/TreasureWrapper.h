#ifndef TREASURE_WRAPPER_H
#define TREASURE_WRAPPER_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

#include "assets/AssetWrapper.h"
#include "CREPixmap.h"

class ResourcesManager;
class TreasureYesNo;

class TreasureWrapper : public AssetTWrapper<treasure> {
    Q_OBJECT

public:
    TreasureWrapper(AssetWrapper *parent, treasure *tr, ResourcesManager *resources);

    virtual QString displayName() const override;
    virtual QIcon displayIcon() const override;
    virtual void displayFillPanel(QWidget *panel) override;

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int child) override;
    virtual int childIndex(AssetWrapper *child) override;

protected:
    ResourcesManager *myResources;
    TreasureYesNo *myNextYes;
    TreasureYesNo *myNextNo;
};

class TreasureYesNo : public AssetWrapper {
public:
    enum YesNo { Yes, No };
    TreasureYesNo(TreasureWrapper *parent, treasure *tr, ResourcesManager *resources, YesNo yesNo);

    virtual QString displayName() const override { return myYesNo == Yes ? "Yes" : "No"; }
    virtual QIcon displayIcon() const override { return myYesNo == Yes ? CREPixmap::getTreasureYesIcon() : CREPixmap::getTreasureNoIcon(); }
    virtual void displayFillPanel(QWidget *panel) override { displayParent()->displayFillPanel(panel); }

    virtual int childrenCount() const override { return 1; }
    virtual AssetWrapper *child(int child) override { return child == 0 ? myWrapped : nullptr; }
    virtual int childIndex(AssetWrapper *child) override { return child == myWrapped ? 0 : -1; }

protected:
    YesNo myYesNo;
    AssetWrapper *myWrapped;
};

#endif /* TREASURE_WRAPPER_H */
