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

#include <string>

/** WebSocket connection state values. */
#define WS_NONE   0   /**< Not a WebSocket connection. */
#define WS_HTTP   1   /**< Accumulating the HTTP Upgrade request. */
#define WS_ACTIVE 2   /**< WebSocket handshake complete, frames in use. */

struct socket_struct;
struct SockList;

struct websocket_state {
    uint8_t  ws_state;           /**< WebSocket state: WS_NONE, WS_HTTP, or WS_ACTIVE. */
    int      ws_header_size;     /**< Current WS frame: expected header bytes (0 = unknown). */
    uint64_t ws_frame_total;     /**< Current WS frame: offset in inbuf where it ends (0 = unknown). */
    size_t   ws_frame_start;     /**< Offset in inbuf where the current frame's header starts. */
    size_t   ws_msg_len;         /**< Payload bytes of a fragmented message collected so far, at inbuf.buf[2..]. */
    bool     ws_in_message;      /**< True while a fragmented message is being reassembled. */
    uint8_t  ws_msg_type;        /**< Type of the last message read: WS_MSG_TEXT or WS_MSG_BINARY. */
};

/** WebSocket message types (the data-frame opcodes of RFC 6455 §5.2). */
#define WS_MSG_TEXT   0x1   /**< Text message. */
#define WS_MSG_BINARY 0x2   /**< Binary message. */

/**
 * Initialize a new websocket connection.
 *
 * @param ns  The newly connected socket.
 * @param host The IP address or hostname of the peer.
 */
void ws_init(socket_struct *ns, const char *host);

/**
 * Attempt to complete the WebSocket HTTP Upgrade handshake for a socket that
 * is in the WS_HTTP state.  Reads available data from the socket and, once a
 * complete HTTP request has been received, validates it (GET method,
 * "Upgrade: websocket", "Connection: Upgrade", Sec-WebSocket-Version 13 and
 * a Sec-WebSocket-Key), sends the HTTP 101 response and transitions the
 * socket to WS_ACTIVE.  An invalid request gets an HTTP error response.
 *
 * Bytes received after the end of the HTTP request are kept in ns->inbuf
 * as the start of the WebSocket stream.
 *
 * @param ns  Socket in WS_HTTP state.
 * @return    True if the connection is still alive (handshake in progress or
 *            just completed; check ws_state to tell them apart), false on
 *            error (socket is set to Ns_Dead).
 */
bool ws_do_handshake(socket_struct *ns);

/**
 * Read one complete WebSocket message from @p ns->fd into @p ns->inbuf.
 *
 * On success (return value 1) ns->inbuf is arranged exactly as
 * SockList_ReadPacket() would leave it: bytes [0..1] contain the big-endian
 * message length and bytes [2..] contain the unmasked payload.
 * ns->inbuf.len is set to 2 + message length, and ns->websocket.ws_msg_type
 * tells whether it was a text or a binary message.
 *
 * Fragmented messages are reassembled.  Ping frames are answered with a
 * Pong, Pong frames are ignored, and a Close frame is echoed back before
 * -1 is returned.  A Close frame with a status code that may not be sent
 * (e.g. 1005) is answered with 1002, and one whose reason is not valid
 * UTF-8 with 1007.  Frames violating RFC 6455 (unmasked, reserved bits set,
 * bad opcode, oversized or fragmented control frames, a message that does
 * not fit in inbuf) are answered with a Close frame and -1 is returned.
 * Text frames are not checked for valid UTF-8.
 *
 * A return value of 0 means not enough data was available yet; the caller
 * should wait for the next select() notification.  Partial data is
 * preserved across calls in ns->inbuf and ns->websocket.
 *
 * @param ns  Active WebSocket socket (ws_state == WS_ACTIVE).
 * @return    1 on complete message, 0 on partial data, -1 on error/close.
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
 * Send a SockList as a WebSocket message of the given type.
 *
 * Like ws_write_frame(), but the frame is sent as @p type, which must be
 * WS_MSG_TEXT or WS_MSG_BINARY.  For text, the payload should be valid UTF-8.
 *
 * @param ns    Active WebSocket socket.
 * @param sl    Buffer to send; the payload is sl->buf[2..sl->len-1].
 * @param type  WS_MSG_TEXT or WS_MSG_BINARY.
 */
void ws_write_message(socket_struct *ns, SockList *sl, uint8_t type);

/**
 * Compute the Sec-WebSocket-Accept value from a Sec-WebSocket-Key.
 *
 * @param key  Sec-WebSocket-Key header value.
 * @return     The Base64-encoded accept value (28 characters).
 */
std::string ws_compute_accept_key(const std::string &key);

#endif /* WEBSOCKET_H */
