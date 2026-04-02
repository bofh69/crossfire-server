#ifndef _SERVER_H
#define _SERVER_H

#include <signal.h>
#include <vector>

extern volatile sig_atomic_t shutdown_flag;

void player_map_change_common(object* op, mapstruct* const oldmap,
                              mapstruct* const newmap);
void login_check_shutdown(object* const op);

bool can_follow(object*, player*);

struct ServerSettings {
    std::vector<std::string> disabled_plugins;  /**< List of disabled plugins, 'All' means all. */
};

extern ServerSettings serverSettings;

#endif
