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
 * WebSocket echo server for use with the Autobahn WebSocket test suite.
 *
 * This server uses the *unmodified* Crossfire WebSocket implementation
 * (socket/websocket.cpp) so that running the Autobahn test suite reveals
 * exactly which RFC 6455 features are currently supported and which are
 * missing.
 *
 * Usage:
 *   ws_echo_server [port]       (default port: 9001)
 *
 * Typical Autobahn run (requires docker or the autobahntestsuite pip package):
 *   ./ws_echo_server &
 *   wstest -m fuzzingclient -s fuzzingclient.json
 *
 * The server listens for plain WebSocket connections (no TLS).  Each
 * connected client has its incoming frames echoed back verbatim using
 * ws_read_packet() and ws_write_frame().  The event loop uses Linux epoll.
 *
 * Known limitations (intentional – this file must not fix them):
 *  - Ping frames are dropped without a Pong reply.
 *  - All frames are echoed with binary opcode (0x2), even text frames.
 *  - Fragmented (continuation) frames close the connection.
 *  - Payloads are limited to MAXSOCKBUF-2 (~65 kB).
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "global.h"
#include "newserver.h"
#include "websocket.h"

/* -------------------------------------------------------------------------
 * Connection management
 * ------------------------------------------------------------------------- */

/** Maximum tracked file descriptor value.  Fds for a test server stay small. */
#define MAX_FD 4096

/** Per-fd connection table; NULL if the slot is unused. */
static socket_struct *conn_by_fd[MAX_FD];

/** Allocate and initialise a new connection struct for @p fd. */
static socket_struct *alloc_conn(int fd, const char *host) {
    socket_struct *ns =
        static_cast<socket_struct *>(calloc(1, sizeof(socket_struct)));
    if (!ns)
        return NULL;
    ns->fd           = fd;
    ns->status       = Ns_Add;
    ns->is_websocket = true;
    ns->websocket.ws_state     = WS_HTTP;
    ns->host         = strdup(host);
    SockList_ResetRead(&ns->inbuf);
    return ns;
}

/** Close the underlying fd, free all resources, and clear the fd table entry. */
static void close_conn(socket_struct *ns, int epfd) {
    epoll_ctl(epfd, EPOLL_CTL_DEL, ns->fd, NULL);
    conn_by_fd[ns->fd] = NULL;
    close(ns->fd);
    free(ns->host);
    free(ns);
}

/* -------------------------------------------------------------------------
 * Listening socket helpers
 * ------------------------------------------------------------------------- */

/**
 * Create a TCP listening socket on @p port.
 *
 * Tries IPv6 dual-stack first; falls back to IPv4 if IPV6_V6ONLY cannot be
 * cleared (some systems require it) or if IPv6 is unavailable.
 */
static int make_listen_socket(int port) {
    int fd = -1;
    int one = 1;

    /* Attempt dual-stack IPv6 (accepts both IPv4 and IPv6 clients). */
    fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd >= 0) {
        int zero = 0;
        if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &zero, sizeof(zero)) < 0) {
            close(fd);
            fd = -1;
        }
    }

    if (fd >= 0) {
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        struct sockaddr_in6 addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin6_family = AF_INET6;
        addr.sin6_port   = htons((uint16_t)port);
        addr.sin6_addr   = in6addr_any;
        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            close(fd);
            fd = -1;
        }
    }

    /* Fallback: plain IPv4. */
    if (fd < 0) {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            perror("socket");
            return -1;
        }
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons((uint16_t)port);
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind");
            close(fd);
            return -1;
        }
    }

    if (listen(fd, 32) < 0) {
        perror("listen");
        close(fd);
        return -1;
    }
    return fd;
}

/** Set a file descriptor to non-blocking mode. */
static void set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0)
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/* -------------------------------------------------------------------------
 * Signal handling
 * ------------------------------------------------------------------------- */

static volatile int g_running = 1;

static void on_signal(int sig) {
    (void)sig;
    g_running = 0;
}

/* -------------------------------------------------------------------------
 * Event handlers
 * ------------------------------------------------------------------------- */

/** Accept one new client from @p listen_fd and register it with @p epfd. */
static void handle_accept(int listen_fd, int epfd) {
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    int client_fd = accept(listen_fd, (struct sockaddr *)&addr, &addrlen);
    if (client_fd < 0)
        return;

    if (client_fd >= MAX_FD) {
        fprintf(stderr, "ws_echo_server: fd %d exceeds MAX_FD, dropping\n",
                client_fd);
        close(client_fd);
        return;
    }

    char host[INET6_ADDRSTRLEN] = "unknown";
    if (addr.ss_family == AF_INET) {
        inet_ntop(AF_INET, &((struct sockaddr_in *)&addr)->sin_addr,
                  host, sizeof(host));
    } else if (addr.ss_family == AF_INET6) {
        inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&addr)->sin6_addr,
                  host, sizeof(host));
    }

    set_nonblock(client_fd);

    socket_struct *ns = alloc_conn(client_fd, host);
    if (!ns) {
        close(client_fd);
        return;
    }

    conn_by_fd[client_fd] = ns;

    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = client_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
}

/**
 * Drive one active client connection.
 *
 * Performs the WebSocket HTTP Upgrade handshake (if still pending), then
 * reads available WebSocket frames and echoes each one back immediately.
 */
static void handle_client(socket_struct *ns, int epfd) {
    /* ------------------------------------------------------------------
     * Phase 1: complete the HTTP Upgrade handshake.
     * ------------------------------------------------------------------ */
    if (ns->websocket.ws_state == WS_HTTP) {
        if (!ws_do_handshake(ns) || ns->status == Ns_Dead) {
            close_conn(ns, epfd);
            return;
        }
        /* Handshake in progress (need more data) or just completed.
         * Either way, wait for the next EPOLLIN. */
        return;
    }

    /* ------------------------------------------------------------------
     * Phase 2: read frames and echo them back.
     * ------------------------------------------------------------------ */
    for (;;) {
        int rc = ws_read_packet(ns);

        if (rc < 0 || ns->status == Ns_Dead) {
            close_conn(ns, epfd);
            return;
        }
        if (rc == 0)
            break; /* No complete frame yet – wait for more data. */

        /* rc == 1: a complete frame is in ns->inbuf.
         * ws_read_packet arranges inbuf so that buf[0..1] holds the
         * big-endian payload length and buf[2..] holds the unmasked
         * payload, which is exactly the SockList layout ws_write_frame
         * expects. */
        ws_write_frame(ns, &ns->inbuf);
        SockList_ResetRead(&ns->inbuf);
    }
}

/* -------------------------------------------------------------------------
 * main
 * ------------------------------------------------------------------------- */

int main(int argc, char **argv) {
    int port = 9001;
    if (argc > 1)
        port = atoi(argv[1]);

    signal(SIGINT,  on_signal);
    signal(SIGTERM, on_signal);
    signal(SIGPIPE, SIG_IGN);

    /* logfile must be set before any LOG() call (e.g. from ws_do_handshake). */
    logfile = stderr;

    int listen_fd = make_listen_socket(port);
    if (listen_fd < 0)
        return 1;

    fprintf(stderr, "ws_echo_server: listening on port %d\n", port);

    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if (epfd < 0) {
        perror("epoll_create1");
        close(listen_fd);
        return 1;
    }

    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    struct epoll_event events[64];
    while (g_running) {
        int n = epoll_wait(epfd, events, 64, 500 /* ms */);
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (events[i].events & (EPOLLHUP | EPOLLERR)) {
                if (fd != listen_fd && conn_by_fd[fd])
                    close_conn(conn_by_fd[fd], epfd);
                continue;
            }

            if (fd == listen_fd) {
                handle_accept(listen_fd, epfd);
            } else {
                socket_struct *ns = conn_by_fd[fd];
                if (ns)
                    handle_client(ns, epfd);
            }
        }
    }

    /* Clean up all open connections. */
    for (int fd = 0; fd < MAX_FD; fd++) {
        if (conn_by_fd[fd])
            close_conn(conn_by_fd[fd], epfd);
    }
    close(epfd);
    close(listen_fd);
    fprintf(stderr, "ws_echo_server: stopped\n");
    return 0;
}
