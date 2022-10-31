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

#ifndef GAME_SOUND_H
#define GAME_SOUND_H

#include "assets/AssetWrapper.h"

class GameSound : public AssetWrapper {
    Q_OBJECT

    Q_PROPERTY(QString description READ description)
public:
    GameSound(AssetWrapper *parent, const QString &name, const QString &description);

    virtual QString displayName() const { return myName; }
    const QString &description() const { return myDescription; }

    virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

private:
    QString myName;
    QString myDescription;
};

#endif /* GAME_SOUND_H */
