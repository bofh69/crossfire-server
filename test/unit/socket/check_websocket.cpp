/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/*
 * Unit tests for the WebSocket transport layer.
 *
 * The WebSocket transport uses a Unix socketpair() to bridge the game engine
 * and the libwebsockets layer for each connected WebSocket client.  These
 * tests validate that the socketpair mechanism behaves correctly:
 *
 *   - data written by the WS proxy side becomes readable on the game side
 *   - data written by the game side becomes readable on the WS proxy side
 *   - closing the WS proxy side signals EOF to the game side
 *   - multiple back-to-back packets can be queued without blocking
 *
 * The WS server lifecycle functions (init_ws_server / free_ws_server /
 * service_ws_connections / ws_server_active) exercise the full libwebsockets
 * context and pull in the game library via websocket.o; those are covered by
 * integration testing rather than this unit test to keep link dependencies
 * minimal.
 */

#include <stdlib.h>
#include <string.h>
#include <check.h>

#ifndef WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

static void setup(void) {
    /* nothing to do */
}

static void teardown(void) {
    /* nothing to do */
}

/**
 * A socketpair (as used for each WS connection) must transfer data
 * written to one end out the other end.  This models the WS proxy side
 * forwarding incoming client data to the game engine.
 */
START_TEST (test_socketpair_ws_to_game)
{
#ifndef WIN32
    int sv[2];
    /* 2-byte big-endian length prefix (5) followed by "hello" */
    const char write_buf[] = "\x00\x05hello";
    char read_buf[32];
    ssize_t n;

    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);

    /* sv[1] = WS proxy side: proxy writes incoming client data here */
    n = write(sv[1], write_buf, sizeof(write_buf) - 1);
    ck_assert_int_eq((int)n, (int)(sizeof(write_buf) - 1));

    /* sv[0] = game side: game reads incoming commands from here */
    n = read(sv[0], read_buf, sizeof(read_buf));
    ck_assert_int_eq((int)n, (int)(sizeof(write_buf) - 1));
    ck_assert_int_eq(memcmp(read_buf, write_buf, n), 0);

    close(sv[0]);
    close(sv[1]);
#endif
}
END_TEST

/**
 * Data written to the game-side fd (sv[0]) must appear on the WS proxy side
 * (sv[1]).  This models the game engine sending a response packet to a WS
 * client.
 */
START_TEST (test_socketpair_game_to_ws)
{
#ifndef WIN32
    int sv[2];
    /* 2-byte big-endian length prefix (7) followed by "goodbye" */
    const char write_buf[] = "\x00\x07goodbye";
    char read_buf[32];
    ssize_t n;

    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);

    /* sv[0] = game side: game writes outgoing data here */
    n = write(sv[0], write_buf, sizeof(write_buf) - 1);
    ck_assert_int_eq((int)n, (int)(sizeof(write_buf) - 1));

    /* sv[1] = WS proxy side: proxy reads outgoing data and sends over WS */
    n = read(sv[1], read_buf, sizeof(read_buf));
    ck_assert_int_eq((int)n, (int)(sizeof(write_buf) - 1));
    ck_assert_int_eq(memcmp(read_buf, write_buf, n), 0);

    close(sv[0]);
    close(sv[1]);
#endif
}
END_TEST

/**
 * Closing the WS proxy side (sv[1]) causes the game side (sv[0]) to read
 * EOF (return value 0).  This is how the game detects that a WebSocket
 * client has disconnected.
 */
START_TEST (test_socketpair_ws_close_signals_eof_to_game)
{
#ifndef WIN32
    int sv[2];
    char read_buf[4];
    ssize_t n;

    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);

    close(sv[1]);  /* WS proxy side closes (WS client disconnected) */

    n = read(sv[0], read_buf, sizeof(read_buf));
    ck_assert_int_eq((int)n, 0);  /* EOF */

    close(sv[0]);
#endif
}
END_TEST

/**
 * Closing the game-side fd (sv[0]) causes the WS proxy side (sv[1]) to read
 * EOF.  This is how the proxy detects that the game has closed the connection
 * (e.g. the player logged out).
 */
START_TEST (test_socketpair_game_close_signals_eof_to_ws)
{
#ifndef WIN32
    int sv[2];
    char read_buf[4];
    ssize_t n;

    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);

    close(sv[0]);  /* Game side closes (player disconnected from game) */

    n = read(sv[1], read_buf, sizeof(read_buf));
    ck_assert_int_eq((int)n, 0);  /* EOF */

    close(sv[1]);
#endif
}
END_TEST

/**
 * Multiple messages can be queued in the socketpair without blocking,
 * simulating back-to-back game packets sent to a WS client.
 */
START_TEST (test_socketpair_multiple_packets_queued)
{
#ifndef WIN32
    int sv[2];
    /* Two minimal packets: 2-byte length (1) + payload */
    const char pkt1[] = "\x00\x01""A";
    const char pkt2[] = "\x00\x01""B";
    char read_buf[16];
    ssize_t n;

    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);

    /* Game writes two packets to sv[0] */
    ck_assert(write(sv[0], pkt1, 3) == 3);
    ck_assert(write(sv[0], pkt2, 3) == 3);

    /* WS proxy can read both from sv[1] in one or more calls */
    n = read(sv[1], read_buf, sizeof(read_buf));
    ck_assert(n >= 3);  /* At least the first packet was received */

    close(sv[0]);
    close(sv[1]);
#endif
}
END_TEST

static Suite *websocket_suite(void)
{
    Suite *s = suite_create("websocket");
    TCase *tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, setup, teardown);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_socketpair_ws_to_game);
    tcase_add_test(tc_core, test_socketpair_game_to_ws);
    tcase_add_test(tc_core, test_socketpair_ws_close_signals_eof_to_game);
    tcase_add_test(tc_core, test_socketpair_game_close_signals_eof_to_ws);
    tcase_add_test(tc_core, test_socketpair_multiple_packets_queued);

    return s;
}

int main(void)
{
    int nf;
    Suite *s = websocket_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_xml(sr, LOGDIR "/unit/socket/websocket.xml");
    srunner_set_log(sr, LOGDIR "/unit/socket/websocket.out");
    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
