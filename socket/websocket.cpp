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
 * \file
 * WebSocket support (RFC 6455) for the Crossfire server.
 *
 * Implements the WebSocket handshake and binary framing layer on top of the
 * existing select()-based event loop.  No external library is required.
 *
 * Protocol note: WebSocket clients send the Crossfire protocol messages as
 * WebSocket binary frame payloads, *without* the 2-byte length prefix that
 * the plain-TCP protocol uses.  That length information is already present
 * in the WebSocket frame header.
 */

#include "global.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <vector>

#ifndef WIN32
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#else
#include <winsock2.h>
#endif

#include "base64.h"
#include "newserver.h"
#include "sha1.h"
#include "shared/newclient.h"
#include "sockproto.h"
#include "sproto.h"
#include "websocket.h"

/* =========================================================================
 * Protocol constants
 * ========================================================================= */

/** Frame opcodes (RFC 6455 §5.2). */
#define WS_OP_CONTINUATION 0x0
#define WS_OP_TEXT         0x1
#define WS_OP_BINARY       0x2
#define WS_OP_CLOSE        0x8
#define WS_OP_PING         0x9
#define WS_OP_PONG         0xA

/** Close status codes (RFC 6455 §7.4.1). */
#define WS_CLOSE_PROTOCOL_ERROR 1002
#define WS_CLOSE_INVALID_DATA   1007
#define WS_CLOSE_TOO_BIG        1009

/** Largest payload a control frame may carry (RFC 6455 §5.5). */
#define WS_MAX_CONTROL_PAYLOAD 125

/**
 * How many control frames (ping/pong) ws_read_packet() handles in one call
 * before yielding, so a client flooding pings cannot monopolise the loop.
 * Any remaining data is still readable, so select() reports it again.
 */
#define WS_MAX_CONTROL_FRAMES_PER_CALL 16

/* =========================================================================
 * Low-level socket helpers
 * ========================================================================= */

/** Log a failed socket operation with the platform's error description. */
static void ws_log_socket_error(const socket_struct *ns, const char *what) {
    const char *host = ns->host ? ns->host : "(unknown)";
#ifdef WIN32
    LOG(llevError, "WebSocket %s failed for %s: WSA error %d\n",
        what, host, WSAGetLastError());
#else
    LOG(llevError, "WebSocket %s failed for %s: %s\n",
        what, host, strerror(errno));
#endif
}

/**
 * Receive up to @p len bytes from @p ns into @p buf.
 *
 * @return Number of bytes read (> 0), 0 if the read would block, or -1 if
 *         the peer closed the connection or an error occurred.
 */
static int ws_recv(socket_struct *ns, uint8_t *buf, size_t len) {
    int n;
#ifdef WIN32
    n = recv(ns->fd, reinterpret_cast<char *>(buf), (int)len, 0);
    if (n < 0) {
        if (WSAGetLastError() == WSAEWOULDBLOCK)
            return 0;
        ws_log_socket_error(ns, "recv");
        return -1;
    }
#else
    do {
        n = (int)read(ns->fd, buf, len);
    } while (n < 0 && errno == EINTR);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        ws_log_socket_error(ns, "read");
        return -1;
    }
#endif
    if (n == 0)
        return -1; /* Orderly shutdown by the peer. */
#ifdef CS_LOGSTATS
    cst_tot.ibytes += n;
    cst_lst.ibytes += n;
#endif
    return n;
}

/**
 * Send all @p len bytes of @p data in one call.
 *
 * Like Write_To_Socket(), a short write is treated as fatal: the caller is
 * expected to mark the socket dead.  Byte counters are only updated for data
 * that was actually sent.
 *
 * @param more  True if more data follows immediately (hint to coalesce).
 * @return True if everything was sent.
 */
static bool ws_send_all(socket_struct *ns, const uint8_t *data, size_t len, bool more) {
#ifdef WIN32
    (void)more;
    int n = send(ns->fd, reinterpret_cast<const char *>(data), (int)len, 0);
    bool ok = n == (int)len;
#else
    int flags = 0;
#ifdef MSG_MORE
    if (more)
        flags |= MSG_MORE;
#else
    (void)more;
#endif
#ifdef MSG_NOSIGNAL
    flags |= MSG_NOSIGNAL;
#endif
    ssize_t n = send(ns->fd, data, len, flags);
    bool ok = n == (ssize_t)len;
#endif
    if (!ok) {
        ws_log_socket_error(ns, "send");
        return false;
    }
#ifdef CS_LOGSTATS
    cst_tot.obytes += len;
    cst_lst.obytes += len;
#endif
    return true;
}

/**
 * Read from the socket until ns->inbuf holds at least @p target bytes.
 * Never reads past @p target, so data belonging to the next frame stays in
 * the kernel buffer.
 *
 * @return 1 when @p target bytes are buffered, 0 if more data is needed,
 *         -1 on error or close.
 */
static int ws_fill(socket_struct *ns, size_t target) {
    SockList *sl = &ns->inbuf;
    if (sl->len >= target)
        return 1;
    assert(target <= sizeof(sl->buf));
    int n = ws_recv(ns, sl->buf + sl->len, target - sl->len);
    if (n <= 0)
        return n;
    sl->len += (size_t)n;
    return sl->len >= target ? 1 : 0;
}

/** Send a control frame (close/ping/pong) with up to 125 bytes of payload. */
static void ws_send_control(socket_struct *ns, uint8_t opcode,
                            const uint8_t *payload, size_t len) {
    uint8_t frame[2 + WS_MAX_CONTROL_PAYLOAD];
    assert(len <= WS_MAX_CONTROL_PAYLOAD);
    frame[0] = (uint8_t)(0x80 | opcode); /* FIN=1 */
    frame[1] = (uint8_t)len;             /* Server frames are never masked. */
    if (len > 0)
        memcpy(frame + 2, payload, len);
    if (!ws_send_all(ns, frame, 2 + len, false))
        ns->status = Ns_Dead;
}

/** Send a Close frame with @p code and report the connection as closed. */
static int ws_fail(socket_struct *ns, uint16_t code, const char *reason) {
    LOG(llevDebug, "WebSocket: closing connection from %s (%u): %s\n",
        ns->host ? ns->host : "(unknown)", code, reason);
    uint8_t payload[2] = { (uint8_t)(code >> 8), (uint8_t)(code & 0xFF) };
    ws_send_control(ns, WS_OP_CLOSE, payload, sizeof(payload));
    return -1;
}

/**
 * Check whether @p code may appear in a Close frame sent by a peer
 * (RFC 6455 §7.4).  1004-1006 and 1015 are reserved and must never be sent;
 * 1012-1014 were registered with IANA later and are accepted.  3000-3999
 * are for libraries and frameworks, 4000-4999 for applications.
 */
static bool ws_close_code_valid(uint16_t code) {
    if (code >= 1000 && code <= 1003)
        return true;
    if (code >= 1007 && code <= 1014)
        return true;
    return code >= 3000 && code <= 4999;
}

/**
 * Check that @p len bytes at @p s are well-formed UTF-8 (RFC 3629):
 * no overlong encodings, no UTF-16 surrogates, nothing above U+10FFFF.
 */
static bool ws_utf8_valid(const uint8_t *s, size_t len) {
    size_t i = 0;
    while (i < len) {
        uint8_t c = s[i];
        if (c < 0x80) {
            i++;
            continue;
        }
        size_t extra;
        uint32_t cp, min;
        if      ((c & 0xE0) == 0xC0) { extra = 1; cp = c & 0x1F; min = 0x80; }
        else if ((c & 0xF0) == 0xE0) { extra = 2; cp = c & 0x0F; min = 0x800; }
        else if ((c & 0xF8) == 0xF0) { extra = 3; cp = c & 0x07; min = 0x10000; }
        else return false; /* Continuation byte or 0xF8-0xFF as a lead byte. */
        if (len - i <= extra)
            return false; /* Truncated sequence. */
        for (size_t k = 1; k <= extra; k++) {
            if ((s[i + k] & 0xC0) != 0x80)
                return false;
            cp = (cp << 6) | (s[i + k] & 0x3F);
        }
        if (cp < min || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF))
            return false;
        i += extra + 1;
    }
    return true;
}

/** Forget any partially received frame or fragmented message. */
static void ws_reset_frame_state(socket_struct *ns) {
    ns->websocket.ws_header_size = 0;
    ns->websocket.ws_frame_total = 0;
    ns->websocket.ws_frame_start = 0;
    ns->websocket.ws_msg_len     = 0;
    ns->websocket.ws_in_message  = false;
}

/* =========================================================================
 * HTTP header parsing
 * ========================================================================= */

/** Case-insensitive comparison of @p len bytes. */
static bool ws_strncaseeq(const char *a, const char *b, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i]))
            return false;
    }
    return true;
}

/**
 * Find a header field in an HTTP request.
 *
 * Only whole field names at the start of a header line are matched (header
 * names are case-insensitive, RFC 7230 §3.2), so text inside another
 * header's value can never be mistaken for a header.
 *
 * @param request    Request containing the terminating blank line
 *                   ("\r\n\r\n").
 * @param name       Field name without the colon.
 * @param[out] value Field value, leading/trailing whitespace removed.
 * @return True if the header was found.
 */
static bool ws_find_header(const std::string &request, const std::string &name,
                           std::string &value) {
    /* Skip the request line. */
    size_t line = request.find("\r\n");
    if (line == std::string::npos)
        return false;
    line += 2;

    for (;;) {
        size_t line_end = request.find("\r\n", line);
        if (line_end == std::string::npos || line_end == line)
            return false; /* End of the header block. */

        if (line_end - line > name.size() && request[line + name.size()] == ':' &&
            ws_strncaseeq(request.data() + line, name.data(), name.size())) {
            size_t v = line + name.size() + 1;
            while (v < line_end && (request[v] == ' ' || request[v] == '\t'))
                v++;
            size_t e = line_end;
            while (e > v && (request[e - 1] == ' ' || request[e - 1] == '\t'))
                e--;
            value = request.substr(v, e - v);
            return true;
        }
        line = line_end + 2;
    }
}

/**
 * Check whether a comma-separated header value contains @p token
 * (case-insensitively), e.g. "keep-alive, Upgrade" contains "upgrade".
 */
static bool ws_header_has_token(const std::string &value, const std::string &token) {
    size_t p = 0;
    while (p < value.size()) {
        while (p < value.size() && (value[p] == ' ' || value[p] == '\t' || value[p] == ','))
            p++;
        size_t start = p;
        while (p < value.size() && value[p] != ',')
            p++;
        size_t stop = p;
        while (stop > start && (value[stop - 1] == ' ' || value[stop - 1] == '\t'))
            stop--;
        if (stop - start == token.size() &&
            ws_strncaseeq(value.data() + start, token.data(), token.size()))
            return true;
    }
    return false;
}

/** Look up a header and check that it contains @p token. */
static bool ws_header_contains(const std::string &request, const std::string &name,
                               const std::string &token) {
    std::string value;
    return ws_find_header(request, name, value) && ws_header_has_token(value, token);
}

/**
 * Refuse the upgrade with an HTTP error response and mark the socket dead.
 *
 * @param status        Status line without CRLF, e.g. "400 Bad Request".
 * @param extra_headers Additional header lines, each ending in CRLF.
 */
static bool ws_reject(socket_struct *ns, const std::string &status,
                      const std::string &extra_headers = "") {
    const std::string response =
        "HTTP/1.1 " + status + "\r\n" +
        extra_headers +
        "Connection: close\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    ws_send_all(ns, reinterpret_cast<const uint8_t *>(response.data()), response.size(), false);
    ns->status = Ns_Dead;
    return false;
}

/* =========================================================================
 * WebSocket accept-key computation
 * ========================================================================= */

/** GUID appended to the client key per RFC 6455 §1.3. */
static const char WS_GUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

std::string ws_compute_accept_key(const std::string &key) {
    const std::string combined = key + WS_GUID;
    uint8_t digest[20];
    sha1(reinterpret_cast<const uint8_t *>(combined.data()), combined.size(), digest);
    return base64_encode(digest, sizeof(digest));
}

void ws_init(socket_struct *ns, const char *host) {
    ns->websocket.ws_state = WS_HTTP;
    ws_reset_frame_state(ns);
    ns->status = Ns_Add;
    free(ns->host);
    ns->host = strdup_local(host);
    SockList_ResetRead(&ns->inbuf);
}

/* =========================================================================
 * HTTP Upgrade handshake
 * ========================================================================= */

/**
 * Apply the X-Forwarded-For header, if any.
 *
 * When the server sits behind a trusted reverse proxy (e.g. nginx), the
 * proxy sets this header to the real client IP.  We parse the first
 * (leftmost) address from the header value, validate it, then run
 * checkbanned() on it so that the IP ban list is honoured even for proxied
 * WebSocket connections.
 *
 * Security note: we only use this header when it is present in the request.
 * The raw TCP peer address was already checked by checkbanned() in
 * do_server() before we reach this point, so the TCP connection itself is
 * always screened first.
 *
 * @return False if the connection must be refused (socket marked dead).
 */
static bool ws_apply_forwarded_for(socket_struct *ns, const std::string &request) {
    std::string xff_hdr;
    if (!ws_find_header(request, "X-Forwarded-For", xff_hdr))
        return true;

    /* Take only the first address (before any comma or whitespace). */
    const std::string xff_ip = xff_hdr.substr(0, xff_hdr.find_first_of(", \t"));

    /* Sanity-check the length: a valid IPv4/IPv6 address fits in 45 bytes
     * (max IPv6 = 39 chars, mapped IPv4 = 45 chars). */
    if (xff_ip.empty() || xff_ip.size() >= 46) {
        LOG(llevInfo, "WebSocket: invalid X-Forwarded-For value from %s\n",
            ns->host);
        return ws_reject(ns, "400 Bad Request");
    }

    /* Validate that every character is a legal IP address character
     * (hex digit, dot, colon, or bracket for IPv6).  This prevents
     * log-injection and any further misuse of the value. */
    for (char c : xff_ip) {
        if (!isxdigit((unsigned char)c) && c != '.' && c != ':' &&
            c != '[' && c != ']') {
            LOG(llevInfo,
                "WebSocket: illegal character in X-Forwarded-For from %s\n",
                ns->host);
            return ws_reject(ns, "400 Bad Request");
        }
    }

    if (checkbanned(NULL, xff_ip.c_str())) {
        LOG(llevInfo,
            "WebSocket: banned X-Forwarded-For address %s (TCP peer %s)\n",
            xff_ip.c_str(), ns->host);
        return ws_reject(ns, "403 Forbidden");
    }

    /* Replace the stored host with the real client IP so that all
     * subsequent log messages and game logic see the correct address. */
    free(ns->host);
    ns->host = strdup_local(xff_ip.c_str());
    LOG(llevDebug, "WebSocket: X-Forwarded-For updated host to %s\n", ns->host);
    return true;
}

bool ws_do_handshake(socket_struct *ns) {

    if (ns->websocket.ws_state != WS_HTTP) {
        return true;
    }

    SockList *sl = &ns->inbuf;

    /* Read available data into inbuf (treat as a plain byte buffer). */
    if (sl->len >= sizeof(sl->buf)) {
        LOG(llevError, "WebSocket HTTP request too large from %s\n", ns->host);
        ns->status = Ns_Dead;
        return false;
    }
    int n = ws_recv(ns, sl->buf + sl->len, sizeof(sl->buf) - sl->len);
    if (n < 0) {
        ns->status = Ns_Dead;
        return false;
    }
    if (n == 0)
        return true; /* Would block; still alive. */
    sl->len += (size_t)n;

    const std::string request(reinterpret_cast<const char *>(sl->buf), sl->len);

    /* Wait until we have the complete HTTP header block (ends with \r\n\r\n). */
    size_t header_end = request.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return true; /* Still alive; need more data. */
    header_end += 4;

    /* Validate the request (RFC 6455 §4.2.1). */
    if (request.compare(0, 4, "GET ") != 0) {
        LOG(llevInfo, "WebSocket: non-GET request from %s\n", ns->host);
        return ws_reject(ns, "405 Method Not Allowed", "Allow: GET\r\n");
    }
    if (!ws_header_contains(request, "Upgrade", "websocket") ||
        !ws_header_contains(request, "Connection", "upgrade")) {
        LOG(llevInfo, "WebSocket: request from %s is not a WebSocket upgrade\n",
            ns->host);
        return ws_reject(ns, "400 Bad Request");
    }
    std::string version;
    if (!ws_find_header(request, "Sec-WebSocket-Version", version) || version != "13") {
        LOG(llevInfo, "WebSocket: unsupported protocol version from %s\n", ns->host);
        return ws_reject(ns, "426 Upgrade Required", "Sec-WebSocket-Version: 13\r\n");
    }

    if (!ws_apply_forwarded_for(ns, request))
        return false;

    /* Extract Sec-WebSocket-Key. */
    std::string ws_key;
    if (!ws_find_header(request, "Sec-WebSocket-Key", ws_key)) {
        LOG(llevError, "WebSocket handshake from %s missing Sec-WebSocket-Key\n",
            ns->host);
        return ws_reject(ns, "400 Bad Request");
    }
    if (ws_key.empty() || ws_key.size() >= 64) {
        LOG(llevError, "WebSocket key invalid length from %s\n", ns->host);
        return ws_reject(ns, "400 Bad Request");
    }

    /* Send HTTP 101 Switching Protocols. */
    const std::string response =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: " + ws_compute_accept_key(ws_key) + "\r\n"
        "\r\n";
    if (!ws_send_all(ns, reinterpret_cast<const uint8_t *>(response.data()), response.size(), false)) {
        ns->status = Ns_Dead;
        return false;
    }

    /* Keep any bytes the client sent after the request; they belong to the
     * WebSocket stream.  init_connection() clears inbuf, so save them. */
    const std::vector<uint8_t> leftover(sl->buf + header_end, sl->buf + sl->len);

    /* Transition to WS_ACTIVE and initialise the Crossfire session. */
    ns->websocket.ws_state = WS_ACTIVE;
    ws_reset_frame_state(ns);

    // init_connection updates ns->host by first freeing it.
    char *tmp_host = ns->host;
    ns->host = NULL;
    init_connection(ns, tmp_host);
    free(tmp_host);

    SockList_ResetRead(&ns->inbuf);
    std::copy(leftover.begin(), leftover.end(), sl->buf);
    sl->len = leftover.size();
    return true;
}

/* =========================================================================
 * WebSocket frame reading
 * ========================================================================= */

int ws_read_packet(socket_struct *ns) {
    if (ns->websocket.ws_state != WS_ACTIVE) {
        return -1;
    }

    SockList *sl = &ns->inbuf;
    websocket_state *ws = &ns->websocket;
    /* Leave room for the NUL that handle_client() appends. */
    const size_t buf_max = sizeof(sl->buf) - 1;

    for (int control_frames = 0; control_frames < WS_MAX_CONTROL_FRAMES_PER_CALL; ) {
        /* A frame's header starts at ws_frame_start: 0 normally, or right
         * after the payload collected so far for a fragmented message. */
        const size_t base = ws->ws_frame_start;

        /* --------------------------------------------------------------
         * Read the first two header bytes and work out the header size.
         * -------------------------------------------------------------- */
        if (base + 2 > buf_max)
            return ws_fail(ns, WS_CLOSE_TOO_BIG, "message too large");
        int r = ws_fill(ns, base + 2);
        if (r <= 0)
            return r;

        const uint8_t byte0 = sl->buf[base];
        const uint8_t byte1 = sl->buf[base + 1];
        const bool    fin    = (byte0 & 0x80) != 0;
        const uint8_t opcode = byte0 & 0x0F;
        const uint8_t plen7  = byte1 & 0x7F;

        if (ws->ws_header_size == 0) {
            /* No extensions are negotiated, so RSV1-3 must be zero. */
            if (byte0 & 0x70)
                return ws_fail(ns, WS_CLOSE_PROTOCOL_ERROR, "reserved bits set");
            /* Every client-to-server frame must be masked (§5.1). */
            if (!(byte1 & 0x80))
                return ws_fail(ns, WS_CLOSE_PROTOCOL_ERROR, "unmasked client frame");
            int hdr = 2 + 4; /* base header + masking key */
            if      (plen7 == 126) hdr += 2;
            else if (plen7 == 127) hdr += 8;
            ws->ws_header_size = hdr;
        }
        const size_t header_size = (size_t)ws->ws_header_size;

        /* --------------------------------------------------------------
         * Read the rest of the header.
         * -------------------------------------------------------------- */
        if (base + header_size > buf_max)
            return ws_fail(ns, WS_CLOSE_TOO_BIG, "message too large");
        r = ws_fill(ns, base + header_size);
        if (r <= 0)
            return r;

        uint64_t payload_len;
        if (plen7 < 126) {
            payload_len = plen7;
        } else if (plen7 == 126) {
            payload_len = ((uint64_t)sl->buf[base + 2] << 8) | sl->buf[base + 3];
        } else {
            payload_len = 0;
            for (int i = 0; i < 8; i++)
                payload_len = (payload_len << 8) | sl->buf[base + 2 + i];
        }

        if (ws->ws_frame_total == 0) {
            /* The most significant bit of a 64-bit length must be 0. */
            if (payload_len >> 63)
                return ws_fail(ns, WS_CLOSE_PROTOCOL_ERROR, "invalid payload length");
            /* Control frames must not be fragmented and carry at most 125
             * bytes (§5.5). */
            if ((opcode & 0x8) && (!fin || payload_len > WS_MAX_CONTROL_PAYLOAD))
                return ws_fail(ns, WS_CLOSE_PROTOCOL_ERROR, "invalid control frame");
            /* Guard against frames that won't fit in our buffer.  Compare
             * the payload length on its own so that the sum below cannot
             * wrap around. */
            const uint64_t max_payload = (uint64_t)(buf_max - base - header_size);
            if (payload_len > max_payload) {
                LOG(llevError, "WebSocket frame too large (%llu byte payload) from %s\n",
                    (unsigned long long)payload_len, ns->host);
                return ws_fail(ns, WS_CLOSE_TOO_BIG, "message too large");
            }
            ws->ws_frame_total = (uint64_t)(base + header_size) + payload_len;
        }

        /* --------------------------------------------------------------
         * Read the payload.
         * -------------------------------------------------------------- */
        r = ws_fill(ns, (size_t)ws->ws_frame_total);
        if (r <= 0)
            return r;

        /* Frame complete: unmask the payload in place. */
        uint8_t *payload = sl->buf + base + header_size;
        const uint8_t *mask = payload - 4;
        for (uint64_t i = 0; i < payload_len; i++)
            payload[i] ^= mask[i & 3];

        ws->ws_header_size = 0;
        ws->ws_frame_total = 0;

        /* --------------------------------------------------------------
         * Control frames may arrive between the fragments of a message.
         * They are handled here and dropped from the buffer.
         * -------------------------------------------------------------- */
        switch (opcode) {
        case WS_OP_CLOSE:
            if (payload_len == 1)
                return ws_fail(ns, WS_CLOSE_PROTOCOL_ERROR, "invalid close frame");
            if (payload_len >= 2) {
                const uint16_t code = (uint16_t)((payload[0] << 8) | payload[1]);
                if (!ws_close_code_valid(code))
                    return ws_fail(ns, WS_CLOSE_PROTOCOL_ERROR, "invalid close code");
                /* The optional reason after the code must be UTF-8. */
                if (!ws_utf8_valid(payload + 2, (size_t)payload_len - 2))
                    return ws_fail(ns, WS_CLOSE_INVALID_DATA, "close reason is not UTF-8");
            }
            /* Echo the status code back to complete the closing handshake. */
            ws_send_control(ns, WS_OP_CLOSE, payload, payload_len >= 2 ? 2 : 0);
            return -1;

        case WS_OP_PING:
            ws_send_control(ns, WS_OP_PONG, payload, (size_t)payload_len);
            if (ns->status == Ns_Dead)
                return -1;
            sl->len = base;
            control_frames++;
            continue;

        case WS_OP_PONG:
            sl->len = base;
            control_frames++;
            continue;

        case WS_OP_CONTINUATION:
            if (!ws->ws_in_message)
                return ws_fail(ns, WS_CLOSE_PROTOCOL_ERROR, "unexpected continuation frame");
            break;

        case WS_OP_TEXT:
        case WS_OP_BINARY:
            if (ws->ws_in_message)
                return ws_fail(ns, WS_CLOSE_PROTOCOL_ERROR, "expected continuation frame");
            /* The first frame decides the type of the whole message. */
            ws->ws_msg_type = opcode;
            break;

        default:
            return ws_fail(ns, WS_CLOSE_PROTOCOL_ERROR, "unsupported opcode");
        }

        /* --------------------------------------------------------------
         * Data frame: append the payload to the message at buf[2..].
         * -------------------------------------------------------------- */
        memmove(sl->buf + 2 + ws->ws_msg_len, payload, (size_t)payload_len);
        ws->ws_msg_len += (size_t)payload_len;

        if (!fin) {
            /* More fragments follow; the next header goes after the data. */
            ws->ws_in_message  = true;
            ws->ws_frame_start = 2 + ws->ws_msg_len;
            sl->len = ws->ws_frame_start;
            continue;
        }

        /* Message complete.  Lay it out as handle_client() expects: the
         * length in buf[0..1] and the payload from buf[2]. */
        const size_t msg_len = ws->ws_msg_len;
        sl->buf[0] = (uint8_t)((msg_len >> 8) & 0xFF);
        sl->buf[1] = (uint8_t)( msg_len       & 0xFF);
        sl->len    = 2 + msg_len;
        ws_reset_frame_state(ns);
        return 1;
    }
    /* Handled enough control frames for now; continue on the next call. */
    return 0;
}

/* =========================================================================
 * WebSocket frame writing
 * ========================================================================= */

void ws_write_frame(socket_struct *ns, SockList *sl) {
    ws_write_message(ns, sl, WS_MSG_BINARY);
}

void ws_write_message(socket_struct *ns, SockList *sl, uint8_t type) {
    if (ns->status == Ns_Dead || sl == NULL || ns->websocket.ws_state != WS_ACTIVE)
        return;
    assert(type == WS_MSG_TEXT || type == WS_MSG_BINARY);

    /* The Crossfire payload is sl->buf[2..sl->len-1]. */
    if (sl->len < 2)
        return;
    size_t payload_len = sl->len - 2;

    /*
     * Build a WebSocket frame header.  Server-to-client frames are never
     * masked (RFC 6455 §6.1).
     *
     * FIN=1, RSV=0, opcode=type (text or binary).
     */
    uint8_t header[4];
    size_t hdr_len;

    header[0] = (uint8_t)(0x80 | type);
    if (payload_len < 126) {
        header[1] = (uint8_t)payload_len;
        hdr_len = 2;
    } else {
        /* payload_len fits in 16 bits because MAXSOCKBUF caps it at 65535. */
        header[1] = 126;
        header[2] = (uint8_t)((payload_len >> 8) & 0xFF);
        header[3] = (uint8_t)( payload_len       & 0xFF);
        hdr_len = 4;
    }

    /*
     * For a 2-byte header, the payload already begins at sl->buf[2], so we
     * can overwrite sl->buf[0..1] with the header and send the whole buffer
     * in a single call.
     *
     * For a 4-byte header there is no in-place room for the extra two header
     * bytes, so the header and payload are sent separately.
     *
     * A send() failure is treated as fatal: the socket is marked Ns_Dead so
     * the main loop closes it on the next pass.
     */
    bool ok;
    if (hdr_len == 2) {
        sl->buf[0] = header[0];
        sl->buf[1] = header[1];
        ok = ws_send_all(ns, sl->buf, sl->len, false);
    } else {
        ok = ws_send_all(ns, header, hdr_len, true) &&
             ws_send_all(ns, sl->buf + 2, payload_len, false);
    }
    if (!ok)
        ns->status = Ns_Dead;
}
