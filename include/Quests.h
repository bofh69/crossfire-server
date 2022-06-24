/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef COMMON_QUESTS_H
#define COMMON_QUESTS_H

#include "quest.h"
#include "AssetsCollection.h"

class Quests : public AssetsCollection<quest_definition> {
public:
    Quests();
    virtual ~Quests();

    size_t visibleCount() const { return visibleQuests; }

protected:
    virtual void replace(quest_definition *existing, quest_definition *update) override;
    virtual void added(quest_definition *quest) override;

private:
    size_t visibleQuests;
};

#endif /* QUESTS_H */

