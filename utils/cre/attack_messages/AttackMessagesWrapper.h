#ifndef ATTACKMESSAGESWRAPPER_H
#define ATTACKMESSAGESWRAPPER_H

#include "../assets/AssetWrapper.h"

class SingleAttackWrapper : public AssetWrapper {
public:
    SingleAttackWrapper(AssetWrapper *parent, uint8_t attackType, const std::string &name)
    : AssetWrapper(parent, "AttackMessage"), myAttackType(attackType), myName(QString::fromStdString(name)) { }
    virtual QString displayName() const { return myName; }
    virtual void displayFillPanel(QWidget *panel) override;

    uint8_t attackType() const { return myAttackType; }

private:
    uint8_t myAttackType;
    QString myName;
};

class AttackMessagesWrapper : public AssetWrapper {
public:
    AttackMessagesWrapper(AssetWrapper *parent);

    virtual QString displayName() const { return "Attack messages"; }

    virtual int childrenCount() const { return myChildren.size(); }
    virtual AssetWrapper *child(int index) { return myChildren[index]; }
    virtual int childIndex(AssetWrapper *asset);

    std::vector<SingleAttackWrapper *> myChildren;
};

#endif /* ATTACKMESSAGESWRAPPER_H */
