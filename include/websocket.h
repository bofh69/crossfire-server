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
 * WebSocket support for the Crossfire server (RFC 6455).
 *
 * WebSocket connections use the same Crossfire protocol as plain TCP
 * connections, except that the 2-byte length prefix is omitted from each
 * message – that information is instead carried in the WebSocket frame
 * header.
 */

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdint.h>
#include <stddef.h>

/** WebSocket connection state values stored in socket_struct::ws_state. */
#define WS_NONE   0   /**< Not a WebSocket connection. */
#define WS_HTTP   1   /**< Accumulating the HTTP Upgrade request. */
#define WS_ACTIVE 2   /**< WebSocket handshake complete, frames in use. */

struct socket_struct;
struct SockList;

/**
 * Create a WebSocket listening socket and add it to the server socket array.
 * Uses settings.ws_port as the port number.  Does nothing if ws_port == 0.
 */
void init_ws_server(void);

/**
 * Attempt to complete the WebSocket HTTP Upgrade handshake for a socket that
 * is in the WS_HTTP state.  Reads available data from the socket and, once a
 * complete HTTP request has been received, sends the HTTP 101 response and
 * transitions the socket to WS_ACTIVE.
 *
 * @param ns  Socket in WS_HTTP state.
 * @return    True if the connection is still alive (handshake in progress or
 *            just completed), false on error (socket is set to Ns_Dead).
 */
bool ws_do_handshake(socket_struct *ns);

/**
 * Read one WebSocket frame from @p ns->fd into @p ns->inbuf.
 *
 * On success (return value 1) ns->inbuf is arranged exactly as
 * SockList_ReadPacket() would leave it: bytes [0..1] contain the big-endian
 * payload length and bytes [2..] contain the unmasked payload.
 * ns->inbuf.len is set to 2 + payload_len.
 *
 * A return value of 0 means not enough data was available yet; the caller
 * should wait for the next select() notification.  Partial frame data is
 * preserved across calls via ns->inbuf, ns->ws_header_size and
 * ns->ws_frame_total.
 *
 * @param ns  Active WebSocket socket (ws_state == WS_ACTIVE).
 * @return    1 on complete frame, 0 on partial data, -1 on error/close.
 */
int ws_read_packet(socket_struct *ns);

/**
 * Send a SockList as a WebSocket binary frame.
 *
 * The payload sent to the client is sl->buf[2..sl->len-1] (the Crossfire
 * protocol message without its 2-byte length prefix).
 *
 * @param ns  Active WebSocket socket.
 * @param sl  Buffer to send.
 */
void ws_write_frame(socket_struct *ns, SockList *sl);

/**
 * Compute the Sec-WebSocket-Accept value from a Sec-WebSocket-Key.
 * The result is a NUL-terminated Base64-encoded string written to @p out.
 *
 * @param key      NUL-terminated Sec-WebSocket-Key header value.
 * @param out      Output buffer.
 * @param out_len  Size of @p out (must be at least 29 bytes).
 */
void ws_compute_accept_key(const char *key, char *out, size_t out_len);

#endif /* WEBSOCKET_H */
