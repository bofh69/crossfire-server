/**
 * @file
 * Defines and structures related to commands the player can send.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

/**
 * One command function.
 * @param op
 * the player executing the command
 * @param params
 * the command parameters; empty string if no commands are given; leading and
 * trailing spaces have been removed
 */
typedef void (*command_function)(object *op, const char *params);

/**
 * One command function, with a custom parameter specified at registration time.
 * @param op
 * the player executing the command
 * @param params
 * the command parameters; empty string if no commands are given; leading and
 * trailing spaces have been removed
 * @param extra
 * extra parameter as specified at registration, with NULL changed to empty string
 */
typedef void (*command_function_extra)(object *op, const char *params, const char *extra);

/** Identifier when registering a command. */
typedef uint64_t command_registration;

/** Standard commands. */
#define COMMAND_TYPE_NORMAL         0
/** Communication commands. */
#define COMMAND_TYPE_COMMUNICATION  1
/** Wizard-only commands. */
#define COMMAND_TYPE_WIZARD         2

enum shutdown_type {
    SHUTDOWN_NONE, //< No shutdown pending
    SHUTDOWN_TIME, //< Shutdown at a given time
    SHUTDOWN_IDLE  //< Shutdown when no active players are in the game
};

struct shutdown_s {
    enum shutdown_type type;
    time_t time;   /**< When using SHUTDOWN_TIME, time of shutdown.
                        When using SHUTDOWN_IDLE, last time server was empty. */
    int next_warn; //<  Used in SHUTDOWN_TIME to warn players
};

extern struct shutdown_s shutdown_state;

#endif /* COMMANDS_H */
