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

#ifndef WIN32
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
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
 * HTTP header search
 * ========================================================================= */

/**
 * Case-insensitive search for @p needle in @p haystack.
 *
 * HTTP header field names are case-insensitive (RFC 7230 §3.2), so a
 * portable replacement for the POSIX-only strcasestr() is needed.
 *
 * @param haystack  NUL-terminated string to search.
 * @param needle    NUL-terminated search string (compared case-insensitively).
 * @return Pointer to the first occurrence, or NULL if not found.
 */
static const char *ws_strcasestr(const char *haystack, const char *needle) {
    size_t needle_len = strlen(needle);
    size_t haystack_len = strlen(haystack);
    for (size_t i = 0; i + needle_len <= haystack_len; i++) {
        bool match = true;
        for (size_t j = 0; j < needle_len; j++) {
            if (tolower((unsigned char)haystack[i + j]) !=
                tolower((unsigned char)needle[j])) {
                match = false;
                break;
            }
        }
        if (match)
            return haystack + i;
    }
    return NULL;
}

/* =========================================================================
 * WebSocket accept-key computation
 * ========================================================================= */

/** GUID appended to the client key per RFC 6455 §1.3. */
static const char WS_GUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

void ws_compute_accept_key(const char *key, char *out, size_t out_len) {
    char combined[128];
    snprintf(combined, sizeof(combined), "%s%s", key, WS_GUID);
    uint8_t digest[20];
    sha1((const uint8_t *)combined, strlen(combined), digest);
    /* 20 bytes → 28 Base64 chars + '=' + NUL = 29 chars minimum */
    assert(out_len >= 29);
    base64_encode(digest, 20, out);
}

/* =========================================================================
 * HTTP Upgrade handshake
 * ========================================================================= */

bool ws_do_handshake(socket_struct *ns) {
    SockList *sl = &ns->inbuf;

    /* Read available data into inbuf (treat as a plain byte buffer). */
    int avail = (int)(sizeof(sl->buf) - 1) - (int)sl->len;
    if (avail <= 0) {
        LOG(llevError, "WebSocket HTTP request too large from %s\n", ns->host);
        ns->status = Ns_Dead;
        return false;
    }

#ifdef WIN32
    int n = recv(ns->fd, reinterpret_cast<char *>(sl->buf + sl->len), avail, 0);
#else
    int n;
    do {
        n = read(ns->fd, sl->buf + sl->len, (size_t)avail);
    } while (n < 0 && errno == EINTR);
#endif

    if (n < 0) {
#ifdef WIN32
        if (WSAGetLastError() == WSAEWOULDBLOCK) return true;
#else
        if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
#endif
        ns->status = Ns_Dead;
        return false;
    }
    if (n == 0) {
        ns->status = Ns_Dead;
        return false;
    }
    sl->len += (size_t)n;
    sl->buf[sl->len] = '\0';

    /* Wait until we have the complete HTTP header block (ends with \r\n\r\n). */
    if (!strstr(reinterpret_cast<char *>(sl->buf), "\r\n\r\n"))
        return true; /* Need more data. */

    /* Extract Sec-WebSocket-Key (case-insensitive per RFC 7230 §3.2). */
    const char *key_hdr = ws_strcasestr(
        reinterpret_cast<char *>(sl->buf), "Sec-WebSocket-Key:");
    if (!key_hdr) {
        LOG(llevError, "WebSocket handshake from %s missing Sec-WebSocket-Key\n",
            ns->host);
        ns->status = Ns_Dead;
        return false;
    }
    key_hdr += strlen("Sec-WebSocket-Key:");
    while (*key_hdr == ' ' || *key_hdr == '\t') key_hdr++;
    const char *key_end = key_hdr;
    while (*key_end && *key_end != '\r' && *key_end != '\n') key_end++;

    size_t key_len = (size_t)(key_end - key_hdr);
    if (key_len == 0 || key_len >= 64) {
        LOG(llevError, "WebSocket key invalid length from %s\n", ns->host);
        ns->status = Ns_Dead;
        return false;
    }

    char ws_key[65];
    memcpy(ws_key, key_hdr, key_len);
    ws_key[key_len] = '\0';

    /* Compute Sec-WebSocket-Accept. */
    char accept[64];
    ws_compute_accept_key(ws_key, accept, sizeof(accept));

    /* Send HTTP 101 Switching Protocols. */
    char response[256];
    int rlen = snprintf(response, sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "\r\n",
        accept);
#ifdef WIN32
    send(ns->fd, response, rlen, 0);
#else
    (void)write(ns->fd, response, (size_t)rlen);
#endif

    /* Transition to WS_ACTIVE and initialise the Crossfire session. */
    ns->ws_state = WS_ACTIVE;
    SockList_ResetRead(&ns->inbuf);
    ns->ws_header_size = 0;
    ns->ws_frame_total = 0;

    init_connection(ns, ns->host);
    return true;
}

/* =========================================================================
 * WebSocket frame reading
 * ========================================================================= */

int ws_read_packet(socket_struct *ns) {
    SockList *sl = &ns->inbuf;

    /* -------------------------------------------------------------------
     * Phase 1: accumulate the first 2 bytes of the frame header so we
     * can determine how many more header bytes we need.
     * ------------------------------------------------------------------- */
    if (ns->ws_header_size == 0 && sl->len < 2) {
        int to_read = 2 - (int)sl->len;
#ifdef WIN32
        int n = recv(ns->fd, reinterpret_cast<char *>(sl->buf + sl->len), to_read, 0);
#else
        int n;
        do {
            n = read(ns->fd, sl->buf + sl->len, (size_t)to_read);
        } while (n < 0 && errno == EINTR);
#endif
        if (n < 0) {
#ifdef WIN32
            if (WSAGetLastError() == WSAEWOULDBLOCK) return 0;
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;
#endif
            return -1;
        }
        if (n == 0)
            return -1;
        sl->len += (size_t)n;
        if (sl->len < 2)
            return 0;
    }

    /* -------------------------------------------------------------------
     * Phase 2: once we have 2 bytes, compute the total header size.
     * Header = 2 bytes base + extended-length bytes + optional 4-byte mask.
     * ------------------------------------------------------------------- */
    if (ns->ws_header_size == 0) {
        bool masked    = (sl->buf[1] & 0x80) != 0;
        uint8_t plen7  =  sl->buf[1] & 0x7F;
        int hdr = 2;
        if      (plen7 == 126) hdr += 2;
        else if (plen7 == 127) hdr += 8;
        if (masked) hdr += 4;
        ns->ws_header_size = hdr;
    }

    /* -------------------------------------------------------------------
     * Phase 3: read until we have the complete frame header.
     * ------------------------------------------------------------------- */
    if (sl->len < (size_t)ns->ws_header_size) {
        int to_read = ns->ws_header_size - (int)sl->len;
#ifdef WIN32
        int n = recv(ns->fd, reinterpret_cast<char *>(sl->buf + sl->len), to_read, 0);
#else
        int n;
        do {
            n = read(ns->fd, sl->buf + sl->len, (size_t)to_read);
        } while (n < 0 && errno == EINTR);
#endif
        if (n < 0) {
#ifdef WIN32
            if (WSAGetLastError() == WSAEWOULDBLOCK) return 0;
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;
#endif
            return -1;
        }
        if (n == 0)
            return -1;
        sl->len += (size_t)n;
        if (sl->len < (size_t)ns->ws_header_size)
            return 0;
    }

    /* -------------------------------------------------------------------
     * Phase 4: parse the payload length and compute the total frame size.
     * ------------------------------------------------------------------- */
    if (ns->ws_frame_total == 0) {
        uint8_t plen7 = sl->buf[1] & 0x7F;
        uint64_t payload_len;
        if (plen7 < 126) {
            payload_len = plen7;
        } else if (plen7 == 126) {
            payload_len = ((uint64_t)sl->buf[2] << 8) | sl->buf[3];
        } else {
            payload_len = 0;
            for (int i = 0; i < 8; i++)
                payload_len = (payload_len << 8) | sl->buf[2 + i];
        }

        uint64_t total = (uint64_t)ns->ws_header_size + payload_len;
        /* Guard against frames that won't fit in our buffer. */
        if (total > (uint64_t)(sizeof(sl->buf) - 1)) {
            LOG(llevError, "WebSocket frame too large (%llu bytes) from %s\n",
                (unsigned long long)total, ns->host);
            return -1;
        }
        ns->ws_frame_total = total;
    }

    /* -------------------------------------------------------------------
     * Phase 5: read the payload.
     * ------------------------------------------------------------------- */
    if (sl->len < ns->ws_frame_total) {
        size_t to_read = (size_t)(ns->ws_frame_total - sl->len);
#ifdef WIN32
        int n = recv(ns->fd, reinterpret_cast<char *>(sl->buf + sl->len), (int)to_read, 0);
#else
        int n;
        do {
            n = read(ns->fd, sl->buf + sl->len, to_read);
        } while (n < 0 && errno == EINTR);
#endif
        if (n < 0) {
#ifdef WIN32
            if (WSAGetLastError() == WSAEWOULDBLOCK) return 0;
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;
#endif
            return -1;
        }
        if (n == 0)
            return -1;
        sl->len += (size_t)n;
        if (sl->len < ns->ws_frame_total)
            return 0;
    }

    /* -------------------------------------------------------------------
     * Frame complete.  Decode it.
     * ------------------------------------------------------------------- */
    uint8_t byte0    = sl->buf[0];
    uint8_t byte1    = sl->buf[1];
    uint8_t opcode   = byte0 & 0x0F;
    bool    masked   = (byte1 & 0x80) != 0;
    uint8_t plen7    = byte1 & 0x7F;

    uint64_t payload_len;
    int ext_bytes = 0;
    if (plen7 < 126) {
        payload_len = plen7;
    } else if (plen7 == 126) {
        payload_len = ((uint64_t)sl->buf[2] << 8) | sl->buf[3];
        ext_bytes = 2;
    } else {
        payload_len = 0;
        for (int i = 0; i < 8; i++)
            payload_len = (payload_len << 8) | sl->buf[2 + i];
        ext_bytes = 8;
    }

    int mask_off    = 2 + ext_bytes;
    int payload_off = mask_off + (masked ? 4 : 0);

    /* Reset frame-tracking state for the next call. */
    ns->ws_header_size = 0;
    ns->ws_frame_total = 0;

    /* Handle control frames. */
    if (opcode == 0x8) { /* Connection Close */
        return -1;
    }
    if (opcode == 0x9 || opcode == 0xA) { /* Ping / Pong – skip */
        sl->len = 0;
        return 0;
    }
    /* Accept text (0x1) and binary (0x2) data frames. */
    if (opcode != 0x1 && opcode != 0x2) {
        LOG(llevDebug, "WebSocket: unsupported opcode 0x%x from %s, closing\n",
            opcode, ns->host);
        return -1;
    }

    /* Unmask the payload in-place. */
    if (masked) {
        const uint8_t *mask = sl->buf + mask_off;
        for (uint64_t i = 0; i < payload_len; i++)
            sl->buf[payload_off + i] ^= mask[i & 3];
    }

    /* Rearrange so the payload sits at buf[2..] with the length in buf[0..1],
     * matching the layout expected by handle_client(). */
    memmove(sl->buf + 2, sl->buf + payload_off, (size_t)payload_len);
    sl->buf[0] = (uint8_t)((payload_len >> 8) & 0xFF);
    sl->buf[1] = (uint8_t)( payload_len       & 0xFF);
    sl->len    = 2 + (size_t)payload_len;

    return 1;
}

/* =========================================================================
 * WebSocket frame writing
 * ========================================================================= */

void ws_write_frame(socket_struct *ns, SockList *sl) {
    if (ns->status == Ns_Dead || sl == NULL)
        return;

    /* The Crossfire payload is sl->buf[2..sl->len-1]. */
    if (sl->len < 2)
        return;
    size_t payload_len = sl->len - 2;

    /*
     * Build a WebSocket binary frame header.  Server-to-client frames are
     * never masked (RFC 6455 §6.1).
     *
     * FIN=1, RSV=0, opcode=0x2 (binary).
     */
    uint8_t header[4];
    int hdr_len;

    header[0] = 0x82; /* FIN=1, opcode=2 */
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
     * Build the complete frame.  For a 2-byte header we can overwrite
     * sl->buf[0..1] directly (same size as the normal length prefix).
     * For a 4-byte header we shift the payload 2 bytes forward; if it
     * happens to overflow the buffer (payload == 65535 bytes, extremely
     * unlikely) we fall back to two separate send() calls.
     */
    if (hdr_len == 2) {
        sl->buf[0] = header[0];
        sl->buf[1] = header[1];
#ifdef WIN32
        send(ns->fd, reinterpret_cast<const char *>(sl->buf), (int)sl->len, 0);
#else
        (void)send(ns->fd, sl->buf, sl->len, 0);
#endif
    } else {
        /* hdr_len == 4 */
        if (4 + payload_len <= sizeof(sl->buf)) {
            memmove(sl->buf + 4, sl->buf + 2, payload_len);
            sl->buf[0] = header[0]; sl->buf[1] = header[1];
            sl->buf[2] = header[2]; sl->buf[3] = header[3];
#ifdef WIN32
            send(ns->fd, reinterpret_cast<const char *>(sl->buf), (int)(4 + payload_len), 0);
#else
            (void)send(ns->fd, sl->buf, 4 + payload_len, 0);
#endif
        } else {
            /* Edge case: payload exactly fills the buffer – send separately. */
#ifdef WIN32
            send(ns->fd, reinterpret_cast<const char *>(header), hdr_len, 0);
            send(ns->fd, reinterpret_cast<const char *>(sl->buf + 2), (int)payload_len, 0);
#else
            (void)send(ns->fd, header, (size_t)hdr_len, MSG_MORE);
            (void)send(ns->fd, sl->buf + 2, payload_len, 0);
#endif
        }
    }
}

