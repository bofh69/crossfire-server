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

#ifndef GAME_SOUNDS_H
#define GAME_SOUNDS_H

#include <vector>
#include "assets/AssetWrapper.h"

class GameSound;

class GameSounds : public AssetWrapper {
    Q_OBJECT

    Q_PROPERTY(QString description READ description)
public:
    GameSounds(AssetWrapper *parent);

    virtual QString displayName() const override { return tr("Game sounds"); }
    virtual int childrenCount() const  override{ return static_cast<int>(mySounds.size()); }
    virtual AssetWrapper *child(int child) override;
    virtual int childIndex(AssetWrapper *child) override;
    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

    QString description() const;

    static std::map<QString, std::string> gameSounds;

private:
    std::vector<GameSound *> mySounds;
};

#endif /* GAME_SOUNDS_H */
