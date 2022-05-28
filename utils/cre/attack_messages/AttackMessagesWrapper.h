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

#ifndef ATTACKMESSAGESWRAPPER_H
#define ATTACKMESSAGESWRAPPER_H

#include "../assets/AssetWrapper.h"

class SingleAttackWrapper : public AssetWrapper {
public:
    SingleAttackWrapper(AssetWrapper *parent, uint8_t attackType, const std::string &name)
    : AssetWrapper(parent, "AttackMessage"), myAttackType(attackType), myName(QString::fromStdString(name)) { }
    virtual QString displayName() const { return myName; }

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
