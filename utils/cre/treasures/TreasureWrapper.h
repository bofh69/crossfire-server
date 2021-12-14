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

    Q_PROPERTY(quint8 chance READ chance WRITE setChance)
    Q_PROPERTY(quint8 magic READ magic WRITE setMagic)
    Q_PROPERTY(quint16 nrof READ nrof WRITE setNrof)
    Q_PROPERTY(const treasurelist *list READ list WRITE setList)
    Q_PROPERTY(const archetype *arch READ arch WRITE setArch)

public:
    TreasureWrapper(AssetWrapper *parent, treasure *tr, ResourcesManager *resources);

    virtual QString displayName() const override;
    virtual QIcon displayIcon() const override;
    virtual void displayFillPanel(QWidget *panel) override;

    virtual int childrenCount() const override;
    virtual AssetWrapper *child(int child) override;
    virtual int childIndex(AssetWrapper *child) override;

    uint8_t chance() const;
    void setChance(uint8_t chance);
    uint8_t magic() const;
    void setMagic(uint8_t magic);
    uint16_t nrof() const;
    void setNrof(uint16_t nrof);
    const treasurelist *list() const;
    void setList(const treasurelist *list);
    const archetype *arch() const;
    void setArch(const archetype *arch);

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

    virtual int childrenCount() const override { return 1; }
    virtual AssetWrapper *child(int child) override { return child == 0 ? myWrapped : nullptr; }
    virtual int childIndex(AssetWrapper *child) override { return child == myWrapped ? 0 : -1; }

protected:
    YesNo myYesNo;
    AssetWrapper *myWrapped;
};

#endif /* TREASURE_WRAPPER_H */
