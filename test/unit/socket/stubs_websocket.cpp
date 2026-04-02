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
 * Minimal stubs required to link the WebSocket unit tests without the full
 * server build.
 */

#include "global.h"
#include "newserver.h"
#include "shared/newclient.h"
#include "sockproto.h"

/* The logger uses the global logfile pointer; point it at stderr so that
 * LOG() calls in error paths do not crash with a NULL file pointer. */
static struct logfile_initializer {
    logfile_initializer() { logfile = stderr; }
} s_logfile_init;

/* -------------------------------------------------------------------------
 * SockList helpers (subset used by the tests and websocket.cpp)
 * ------------------------------------------------------------------------- */

void SockList_Init(SockList *sl) { SockList_Reset(sl); }
void SockList_Term(SockList *sl) { (void)sl; }

void SockList_Reset(SockList *sl)      { sl->len = 2; }
void SockList_ResetRead(SockList *sl)  { sl->len = 0; }

static void SockList_Ensure(const SockList *sl, size_t size) {
    if (sl->len + size > sizeof(sl->buf))
        fatal(OUT_OF_MEMORY);
}

void SockList_AddChar(SockList *sl, unsigned char data) {
    SockList_Ensure(sl, 1);
    sl->buf[sl->len++] = data;
}

void SockList_AddData(SockList *sl, const void *data, size_t len) {
    SockList_Ensure(sl, len);
    memcpy(sl->buf + sl->len, data, len);
    sl->len += len;
}

void SockList_AddString(SockList *sl, const char *data) {
    SockList_AddData(sl, data, strlen(data));
}

void SockList_NullTerminate(SockList *sl) {
    SockList_Ensure(sl, 1);
    sl->buf[sl->len] = '\0';
}

size_t SockList_Avail(const SockList *sl) {
    return sizeof(sl->buf) - sl->len;
}

/* -------------------------------------------------------------------------
 * Server stubs not needed for these tests
 * ------------------------------------------------------------------------- */

/** Called by ws_do_handshake() after handshake completes – not tested here. */
void init_connection(socket_struct *ns, const char *from_ip) {
    (void)ns; (void)from_ip;
}

/** Server-only function used in lowlevel.cpp – not linked here. */
int count_players(void) { return 0; }

/**
 * Stub for checkbanned().  By default returns 0 (not banned).
 * Tests that need to simulate a banned IP override g_checkbanned_result.
 */
int g_checkbanned_result = 0;      /* 0 = allow, 1 = banned */
char g_checkbanned_host[64] = "";  /* last host passed to checkbanned() */

int checkbanned(const char *login, const char *host) {
    (void)login;
    if (host)
        snprintf(g_checkbanned_host, sizeof(g_checkbanned_host), "%s", host);
    return g_checkbanned_result;
}
