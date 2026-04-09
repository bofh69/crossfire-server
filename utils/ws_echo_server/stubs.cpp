/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2026 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * Minimal stubs required to link ws_echo_server without the full server build.
 *
 * ws_echo_server links against libcross (SHA-1, Base64, LOG, fatal, …) but
 * not against libcfserver.  The server-side functions referenced by
 * socket/websocket.cpp are stubbed out here.
 */

#include <string.h>

#include "global.h"
#include "newserver.h"
#include "shared/newclient.h"

/* Point the logger at stderr so LOG() calls in websocket.cpp don't crash. */
static struct LogfileInit {
    LogfileInit() { logfile = stderr; }
} s_logfile_init;

/* -------------------------------------------------------------------------
 * SockList stubs
 *
 * socket/lowlevel.cpp (part of libcfserver, not libcross) defines these.
 * Provide the minimal subset that websocket.cpp uses.
 * ------------------------------------------------------------------------- */

void SockList_Init(SockList *sl)       { sl->len = 2; }
void SockList_Term(SockList *sl)       { (void)sl; }
void SockList_Reset(SockList *sl)      { sl->len = 2; }
void SockList_ResetRead(SockList *sl)  { sl->len = 0; }

size_t SockList_Avail(const SockList *sl) {
    return sizeof(sl->buf) - sl->len;
}

/* -------------------------------------------------------------------------
 * Server-specific stubs
 * ------------------------------------------------------------------------- */

/**
 * Called by ws_do_handshake() after the HTTP Upgrade handshake completes.
 * The echo server needs no Crossfire session initialisation.
 */
void init_connection(socket_struct *ns, const char *from_ip) {
    (void)ns;
    (void)from_ip;
}

/**
 * Ban-list check.  The echo server accepts all connections.
 */
int checkbanned(const char *login, const char *host) {
    (void)login;
    (void)host;
    return 0; /* not banned */
}
