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

/**
 * \file
 * WebSocket transport layer.
 *
 * \date 2024
 *
 * Implements a WebSocket server that mirrors the existing binary protocol.
 * For each WebSocket connection, a Unix socketpair is created. The game-side
 * fd is added to init_sockets[] and handled by the normal game loop. The
 * proxy-side fd bridges data between the WebSocket client and the game engine.
 *
 * Clients connect via WebSocket and speak the same binary protocol as TCP
 * clients: each message is a 2-byte big-endian length followed by the payload.
 */

#include "global.h"

#ifdef HAVE_LIBWEBSOCKETS

#include "sproto.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <libwebsockets.h>
#include "newserver.h"
#include "loader.h"

/** Global libwebsockets context for the WebSocket server. */
static struct lws_context *ws_context = NULL;

/**
 * Per-WebSocket-session data stored by libwebsockets.
 *
 * game_socket_idx is the index into init_sockets[] for this connection,
 * or -1 if the game side has already been closed.
 */
struct ws_session_data {
    int game_socket_idx;
};

/**
 * Find or allocate a free slot in init_sockets[].
 *
 * Replicates the slot-finding logic from doeric_server() so that the
 * WebSocket callback can register new connections independently.
 *
 * @return index of a free slot, or -1 if allocation failed.
 */
static int get_free_socket_slot(void)
{
    int newsocknum;

    if (socket_info.allocated_sockets <= socket_info.nconns) {
        init_sockets = static_cast<socket_struct *>(realloc(init_sockets,
                               sizeof(socket_struct) * (socket_info.nconns + 1)));
        if (!init_sockets)
            fatal(OUT_OF_MEMORY);
        newsocknum = socket_info.allocated_sockets;
        socket_info.allocated_sockets++;
        init_sockets[newsocknum].faces_sent_len = nrofpixmaps;
        init_sockets[newsocknum].faces_sent =
            static_cast<uint8_t *>(calloc(1, nrofpixmaps * sizeof(*init_sockets[newsocknum].faces_sent)));
        if (!init_sockets[newsocknum].faces_sent)
            fatal(OUT_OF_MEMORY);
        init_sockets[newsocknum].status = Ns_Avail;
        init_sockets[newsocknum].ws_proxy_fd = -1;
        init_sockets[newsocknum].is_websocket = 0;
        init_sockets[newsocknum].wsi = NULL;
    } else {
        int j;
        newsocknum = -1;
        for (j = 1; j < socket_info.allocated_sockets; j++) {
            if (init_sockets[j].status == Ns_Avail) {
                newsocknum = j;
                break;
            }
        }
    }
    return newsocknum;
}

/**
 * libwebsockets callback for the crossfire binary protocol over WebSocket.
 *
 * Handles connection establishment, data reception, data sending, and
 * connection closure for WebSocket clients.
 */
static int ws_callback(struct lws *wsi, enum lws_callback_reasons reason,
                       void *user, void *in, size_t len)
{
    struct ws_session_data *sd = static_cast<struct ws_session_data *>(user);
    socket_struct *ns;

    switch (reason) {

    case LWS_CALLBACK_ESTABLISHED: {
        char ip_buf[64] = "unknown";
        int sv[2];
        int newsocknum;

        lws_get_peer_simple(wsi, ip_buf, sizeof(ip_buf));

        /* Create the socketpair bridge between WS and the game engine */
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
            LOG(llevError, "ws_callback: socketpair failed: %s\n", strerror(errno));
            return -1;
        }

        /* Make the proxy (WS) side non-blocking */
        if (fcntl(sv[1], F_SETFL, O_NONBLOCK) < 0) {
            LOG(llevError, "ws_callback: fcntl sv[1] failed: %s\n", strerror(errno));
            close(sv[0]);
            close(sv[1]);
            return -1;
        }

        newsocknum = get_free_socket_slot();
        if (newsocknum < 0) {
            LOG(llevError, "ws_callback: no free socket slot\n");
            close(sv[0]);
            close(sv[1]);
            return -1;
        }

        if (checkbanned(NULL, ip_buf)) {
            LOG(llevInfo, "Banned host tried to connect (WebSocket): [%s]\n", ip_buf);
            close(sv[0]);
            close(sv[1]);
            /* slot stays Ns_Avail */
            return -1;
        }

        ns = &init_sockets[newsocknum];
        ns->fd = sv[0];
        ns->ws_proxy_fd = sv[1];
        ns->is_websocket = 1;
        ns->wsi = wsi;

        sd->game_socket_idx = newsocknum;

        init_connection(ns, ip_buf);
        socket_info.nconns++;

        LOG(llevDebug, "ws_callback: new WebSocket connection from %s (slot %d)\n",
            ip_buf, newsocknum);
        break;
    }

    case LWS_CALLBACK_RECEIVE: {
        if (sd->game_socket_idx < 0)
            return 0;

        ns = &init_sockets[sd->game_socket_idx];
        if (ns->status == Ns_Dead)
            return -1;

        /* Forward raw payload (including 2-byte length prefix) to game side */
        if (write(ns->ws_proxy_fd, in, len) < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                LOG(llevError, "ws_callback: write to proxy fd failed: %s\n",
                    strerror(errno));
                ns->status = Ns_Dead;
                return -1;
            }
        }
        break;
    }

    case LWS_CALLBACK_SERVER_WRITEABLE: {
        unsigned char buf[LWS_PRE + MAXSOCKSENDBUF + 4];
        int n;

        if (sd->game_socket_idx < 0)
            return -1;  /* Game side gone; close WS connection */

        ns = &init_sockets[sd->game_socket_idx];
        if (ns->status == Ns_Dead)
            return -1;

        /* Read data that the game engine wrote to the game-side socket */
        n = recv(ns->ws_proxy_fd, buf + LWS_PRE, sizeof(buf) - LWS_PRE, 0);
        if (n == 0) {
            return -1;  /* EOF: game side closed */
        }
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return 0;  /* No data available right now */
            return -1;
        }

        if (lws_write(wsi, buf + LWS_PRE, n, LWS_WRITE_BINARY) < 0)
            return -1;

        break;
    }

    case LWS_CALLBACK_CLOSED: {
        if (sd->game_socket_idx >= 0) {
            ns = &init_sockets[sd->game_socket_idx];
            ns->wsi = NULL;  /* WS connection is already gone */
            ns->status = Ns_Dead;
        }
        break;
    }

    default:
        break;
    }

    return 0;
}

/** WebSocket protocol table for libwebsockets. */
static struct lws_protocols ws_protocols[] = {
    {
        "crossfire",    /* protocol name */
        ws_callback,
        sizeof(struct ws_session_data),
        MAXSOCKRECVBUF, /* per-session buffer size hint */
    },
    { NULL, NULL, 0, 0 }  /* terminator */
};

/**
 * Initialize the WebSocket server.
 *
 * Creates a libwebsockets context listening on the given port. If port is 0,
 * WebSocket support is disabled and this function returns 0 immediately.
 *
 * @param port  TCP port to listen on, or 0 to disable WebSocket support.
 * @return 0 on success, -1 on failure.
 */
int init_ws_server(int port)
{
    struct lws_context_creation_info info;

    if (port == 0) {
        return 0;  /* WebSocket support disabled */
    }

    memset(&info, 0, sizeof(info));
    info.port = port;
    info.protocols = ws_protocols;
    info.options = LWS_SERVER_OPTION_DISABLE_IPV6;
    info.gid = -1;
    info.uid = -1;

    ws_context = lws_create_context(&info);
    if (!ws_context) {
        LOG(llevError, "init_ws_server: failed to create WebSocket context on port %d\n",
            port);
        return -1;
    }

    LOG(llevInfo, "WebSocket server listening on port %d\n", port);
    return 0;
}

/**
 * Service pending WebSocket events (non-blocking poll).
 *
 * Should be called regularly from the main game loop to handle incoming
 * WebSocket data and send queued outgoing data.
 */
void service_ws_connections(void)
{
    if (ws_context)
        lws_service(ws_context, 0);
}

/**
 * Request that a WebSocket connection flush its output buffer.
 *
 * Called when the game engine has written data to the game-side fd of a
 * WebSocket connection's socketpair, making the proxy-side fd readable.
 *
 * @param ns  The socket_struct for the WebSocket connection.
 */
void ws_request_write(socket_struct *ns)
{
    if (ns->is_websocket && ns->wsi)
        lws_callback_on_writable(static_cast<struct lws *>(ns->wsi));
}

/**
 * Prepare a WebSocket connection for game-side closure.
 *
 * Called from free_newsocket() before closing the socketpair fds. Marks the
 * per-session data so that subsequent WebSocket callbacks know the game side
 * is gone, and schedules a writeable callback so libwebsockets will close the
 * WebSocket connection cleanly.
 *
 * @param ns  The socket_struct being freed.
 */
void ws_cleanup_connection(socket_struct *ns)
{
    struct ws_session_data *sd;

    if (!ns->is_websocket || !ns->wsi)
        return;

    sd = static_cast<struct ws_session_data *>(lws_wsi_user(static_cast<struct lws *>(ns->wsi)));
    if (sd)
        sd->game_socket_idx = -1;

    /* Schedule a writeable callback; returning -1 from it will close the WS */
    lws_callback_on_writable(static_cast<struct lws *>(ns->wsi));
    ns->wsi = NULL;
}

/**
 * Check whether the WebSocket server is active.
 *
 * @return 1 if a WebSocket server context exists, 0 otherwise.
 */
int ws_server_active(void)
{
    return ws_context != NULL;
}

/**
 * Shut down and free the WebSocket server.
 */
void free_ws_server(void)
{
    if (ws_context) {
        lws_context_destroy(ws_context);
        ws_context = NULL;
        LOG(llevDebug, "WebSocket server stopped\n");
    }
}

#endif /* HAVE_LIBWEBSOCKETS */
