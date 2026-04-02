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

#ifndef QUEST_H
#define QUEST_H

#include "global.h"
#include <vector>

/** One condition to automatically move to a quest step. */
struct quest_condition {
    sstring quest_code = nullptr;   /**< The quest that triggers the condition */
    int minstep = 0;                /**< The earliest step in the quest that triggers the condition,
                                        -1 means finished, 0 means not started */
    int maxstep = 0;                /**< The latest step that triggers the condition, to match,
                                        the stages must be between minstep and maxstep */
};

/** One step of a quest. */
struct quest_step_definition {
    int step = 0;                               /**< Step identifier. */
    sstring step_description = nullptr;         /**< Step description to show player. */
    bool is_completion_step:1;                   /**< Whether this step completes the quest (1) or not (0) */
    std::vector<quest_condition *> conditions;  /**< The conditions that must be satisfied to trigger the step */
};

/** Definition of an in-game quest. */
struct quest_definition {
    sstring quest_code = nullptr;           /**< Quest internal code. */
    sstring quest_title = nullptr;          /**< Quest title for player. */
    sstring quest_description = nullptr;    /**< Quest longer description. */
    sstring quest_comment = nullptr;        /**< Quest comment, not visible to players. */
    int quest_restart = 0;                  /**< If non zero, can be restarted. */
    const Face *face = nullptr;             /**< Face associated with this quest. */
    uint32_t client_code = 0;               /**< The code used to communicate with the client, merely a unique index. */
    bool quest_is_system = false;           /**< If set then the quest isn't counted or listed. */
    std::vector<quest_step_definition *> steps; /**< Quest steps. */
    struct quest_definition *parent = nullptr;  /**< Parent for this quest, NULL if it is a 'top-level' quest */
};

typedef void(*quest_op)(const quest_definition *, void *);

void quest_for_each(quest_op op, void *user);

/**
 * Allocate a quest_step_definition, will call fatal() if out of memory.
 * @return new structure.
 */
quest_step_definition *quest_create_step(void);

/**
 * Allocate a quest_condition, will call fatal() if out of memory.
 * @return new structure.
 */
quest_condition *quest_create_condition(void);

quest_definition *quest_create(const char *name);
void quest_destroy_condition(quest_condition *condition);
void quest_destroy_step(quest_step_definition *step);
void quest_clear(quest_definition *quest);
void quest_destroy(quest_definition *quest);

/**
 * Parse a single step condition.
 * This may be expressed as one of the following:
 * - questcode 20 (the quest questcode must be at step 20)
 * - questcode <=20 (the quest questcode must not be beyond step 20)
 * - questcode 10-20 (the quest questcode must be between steps 10 and 20)
 * - questcode finished (the quest questcode must have been completed)
 *
 * @param condition condition to fill.
 * @param buffer where to read from.
 * @return 1 if the condition was parsed, 0 else.
 */
int quest_condition_from_string(quest_condition *condition, const char *buffer);

/**
 * Write a step condition to a buffer. If the buffer is too small, the line is truncated.
 * @param buf where to write.
 * @param len length of buf.
 * @param condition item to write, must not be NULL.
 */
void quest_write_condition(char *buf, size_t len, const quest_condition *condition);

#endif /* QUEST_H */
