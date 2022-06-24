/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020-2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "global.h"
#include "AssetsCollection.h"

class AllAnimations : public AssetsCollection<Animations> {
public:
    AllAnimations();

protected:
    virtual void replace(Animations *existing, Animations *update) override;
    virtual void added(Animations *anim) override;
};

#endif /* ANIMATIONS_H */
