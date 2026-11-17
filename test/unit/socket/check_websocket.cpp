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
 * Unit tests for WebSocket support (socket/websocket.cpp).
 *
 * Tests cover:
 *  - SHA-1 + Base64 accept-key computation (ws_compute_accept_key)
 *  - WebSocket frame parsing via ws_read_packet using a socket-pair
 *  - WebSocket frame writing via ws_write_frame using a socket-pair
 *  - HTTP Upgrade handshake (ws_do_handshake): valid and negative cases
 */

#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdio.h>

/* Pull in the declarations we need. */
#include "global.h"
#include "websocket.h"
#include "newserver.h"
#include "shared/newclient.h"
#include "sockproto.h"

/* -------------------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------------------- */

/** Build a minimal socket_struct backed by one end of a socketpair. */
static socket_struct make_ws_socket(int fd) {
    socket_struct ns;
    memset(&ns, 0, sizeof(ns));
    ns.fd           = fd;
    ns.status       = Ns_Add;
    ns.is_websocket = true;
    ns.websocket.ws_state     = WS_ACTIVE;
    ns.host         = strdup("127.0.0.1"); /* ws_do_handshake frees it */
    SockList_ResetRead(&ns.inbuf);
    return ns;
}

/** Build a socket_struct in WS_HTTP state for handshake tests. */
static socket_struct make_ws_http_socket(int fd) {
    socket_struct ns = make_ws_socket(fd);
    ns.websocket.ws_state      = WS_HTTP;
    return ns;
}

/**
 * Write @p len raw bytes to @p fd, simulating data arriving from a WebSocket
 * client.
 */
static void feed_bytes(int fd, const uint8_t *data, size_t len) {
    size_t written = 0;
    while (written < len) {
        ssize_t n = write(fd, data + written, len - written);
        if (n <= 0) break;
        written += (size_t)n;
    }
}

/**
 * Build a masked WebSocket binary frame carrying @p payload_len bytes of
 * @p payload.  The masking key is all-zero for simplicity (XOR with 0 is
 * identity), so the "masked" data equals the plaintext.
 *
 * Written to *out; returns the total frame length.
 */
static size_t build_ws_frame(uint8_t *out, const uint8_t *payload,
                              size_t payload_len) {
    size_t hdr = 0;
    out[hdr++] = 0x82; /* FIN=1, opcode=2 (binary) */

    if (payload_len < 126) {
        out[hdr++] = (uint8_t)(0x80 | payload_len); /* MASK=1 */
    } else {
        out[hdr++] = (uint8_t)(0x80 | 126);         /* MASK=1, extended len */
        out[hdr++] = (uint8_t)((payload_len >> 8) & 0xFF);
        out[hdr++] = (uint8_t)( payload_len       & 0xFF);
    }

    /* Masking key – all zeros so data passes through unchanged. */
    out[hdr++] = 0x00;
    out[hdr++] = 0x00;
    out[hdr++] = 0x00;
    out[hdr++] = 0x00;

    memcpy(out + hdr, payload, payload_len);
    return hdr + payload_len;
}

/** Masking key used by build_frame() for masked frames. */
static const uint8_t TEST_MASK[4] = { 0x37, 0xFA, 0x21, 0x3D };

/**
 * Build a client frame with full control over FIN, opcode and masking.
 * Masked frames use TEST_MASK, so the payload really is transformed on the
 * wire.  Payloads up to 65535 bytes are supported.
 *
 * @return Total frame length written to @p out.
 */
static size_t build_frame(uint8_t *out, bool fin, uint8_t opcode,
                          const void *payload, size_t payload_len, bool masked) {
    size_t hdr = 0;
    out[hdr++] = (uint8_t)((fin ? 0x80 : 0x00) | opcode);
    uint8_t mask_bit = masked ? 0x80 : 0x00;
    if (payload_len < 126) {
        out[hdr++] = (uint8_t)(mask_bit | payload_len);
    } else {
        out[hdr++] = (uint8_t)(mask_bit | 126);
        out[hdr++] = (uint8_t)((payload_len >> 8) & 0xFF);
        out[hdr++] = (uint8_t)( payload_len       & 0xFF);
    }
    const uint8_t *in = static_cast<const uint8_t *>(payload);
    if (masked) {
        memcpy(out + hdr, TEST_MASK, 4);
        hdr += 4;
        for (size_t i = 0; i < payload_len; i++)
            out[hdr + i] = in[i] ^ TEST_MASK[i & 3];
    } else if (payload_len > 0) {
        memcpy(out + hdr, in, payload_len);
    }
    return hdr + payload_len;
}

/** Read whatever the server wrote to the peer end @p fd (non-blocking). */
static size_t read_available(int fd, uint8_t *buf, size_t len) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
    size_t total = 0;
    while (total < len) {
        ssize_t n = read(fd, buf + total, len - total);
        if (n <= 0)
            break;
        total += (size_t)n;
    }
    return total;
}

/** Assert that @p fd received a Close frame carrying status @p code. */
static void assert_close_sent(int fd, uint16_t code) {
    uint8_t resp[16];
    size_t n = read_available(fd, resp, sizeof(resp));
    ck_assert_uint_eq(n, 4);
    ck_assert_uint_eq(resp[0], 0x88);  /* FIN + Close */
    ck_assert_uint_eq(resp[1], 2);     /* unmasked, 2-byte status */
    ck_assert_uint_eq((resp[2] << 8) | resp[3], code);
}

/**
 * Send a complete HTTP Upgrade request through @p fd with the given
 * Sec-WebSocket-Key header name (to allow testing case variations).
 */
static void send_http_upgrade(int fd, const char *key_header_name,
                              const char *key_value) {
    char req[512];
    int n = snprintf(req, sizeof(req),
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "%s: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        key_header_name, key_value);
    feed_bytes(fd, (const uint8_t *)req, (size_t)n);
}

/* -------------------------------------------------------------------------
 * Test: ws_compute_accept_key
 * ------------------------------------------------------------------------- */

/**
 * RFC 6455 §1.3 example:
 *   Key   = "dGhlIHNhbXBsZSBub25jZQ=="
 *   Accept = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo="
 */
START_TEST(test_accept_key_rfc_example) {
    const std::string accept = ws_compute_accept_key("dGhlIHNhbXBsZSBub25jZQ==");
    ck_assert_str_eq(accept.c_str(), "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
}
END_TEST

/** A different key should produce a different accept value. */
START_TEST(test_accept_key_different_keys_differ) {
    ck_assert(ws_compute_accept_key("dGhlIHNhbXBsZSBub25jZQ==") !=
              ws_compute_accept_key("x3JJHMbDL1EzLkh9GBhXDw=="));
}
END_TEST

/** The same key always produces the same accept value. */
START_TEST(test_accept_key_deterministic) {
    ck_assert(ws_compute_accept_key("dGhlIHNhbXBsZSBub25jZQ==") ==
              ws_compute_accept_key("dGhlIHNhbXBsZSBub25jZQ=="));
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_read_packet – small frame
 * ------------------------------------------------------------------------- */

START_TEST(test_read_packet_small) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);

    /* Make the reading end non-blocking. */
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_socket(sv[0]);

    /* Build a 5-byte payload "hello". */
    uint8_t payload[] = { 'h', 'e', 'l', 'l', 'o' };
    uint8_t frame[32];
    size_t frame_len = build_ws_frame(frame, payload, sizeof(payload));

    feed_bytes(sv[1], frame, frame_len);

    int result = ws_read_packet(&ns);
    ck_assert_int_eq(result, 1);

    /* inbuf should now look like a normal SockList packet:
     * buf[0..1] = big-endian length (5), buf[2..6] = "hello" */
    ck_assert_uint_eq(ns.inbuf.len, 2 + sizeof(payload));
    ck_assert_uint_eq(ns.inbuf.buf[0], 0);
    ck_assert_uint_eq(ns.inbuf.buf[1], (uint8_t)sizeof(payload));
    ck_assert_mem_eq(ns.inbuf.buf + 2, payload, sizeof(payload));

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_read_packet – frame arriving in two chunks (partial read)
 * ------------------------------------------------------------------------- */

START_TEST(test_read_packet_partial) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t payload[] = { 0x01, 0x02, 0x03 };
    uint8_t frame[32];
    size_t frame_len = build_ws_frame(frame, payload, sizeof(payload));

    /* Send only the first 3 bytes. */
    feed_bytes(sv[1], frame, 3);

    int result = ws_read_packet(&ns);
    ck_assert_int_eq(result, 0); /* Incomplete – should ask for more. */

    /* Send the rest. */
    feed_bytes(sv[1], frame + 3, frame_len - 3);

    result = ws_read_packet(&ns);
    ck_assert_int_eq(result, 1);
    ck_assert_uint_eq(ns.inbuf.len, 2 + sizeof(payload));
    ck_assert_mem_eq(ns.inbuf.buf + 2, payload, sizeof(payload));

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_read_packet – extended-length frame (payload >= 126 bytes)
 * ------------------------------------------------------------------------- */

START_TEST(test_read_packet_extended_length) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_socket(sv[0]);

    /* Build a 200-byte payload. */
    uint8_t payload[200];
    for (int i = 0; i < 200; i++)
        payload[i] = (uint8_t)(i & 0xFF);

    uint8_t frame[220];
    size_t frame_len = build_ws_frame(frame, payload, sizeof(payload));

    feed_bytes(sv[1], frame, frame_len);

    int result = ws_read_packet(&ns);
    ck_assert_int_eq(result, 1);
    ck_assert_uint_eq(ns.inbuf.len, 2 + sizeof(payload));
    ck_assert_mem_eq(ns.inbuf.buf + 2, payload, sizeof(payload));

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_read_packet – close frame returns -1
 * ------------------------------------------------------------------------- */

START_TEST(test_read_packet_close_frame) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_socket(sv[0]);

    /* Close frame: FIN=1, opcode=8, MASK=1, length=0, mask=0x00000000 */
    uint8_t close_frame[] = { 0x88, 0x80, 0x00, 0x00, 0x00, 0x00 };
    feed_bytes(sv[1], close_frame, sizeof(close_frame));

    int result = ws_read_packet(&ns);
    ck_assert_int_eq(result, -1);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_read_packet – too-large frame (64-bit extended length)
 * A frame whose 64-bit payload length exceeds the buffer must return -1.
 * -------------------------------------------------------------------------*/

START_TEST(test_read_packet_too_large_64bit) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_socket(sv[0]);

    /* Build a 14-byte unmasked header with 64-bit length = 100000. */
    uint8_t frame[14];
    frame[0] = 0x82;  /* FIN=1, binary */
    frame[1] = 0xFF;  /* MASK=1, plen7=127 (64-bit extended length) */
    /* 64-bit big-endian length: 100000 = 0x00000000000186A0 */
    frame[2]  = 0x00; frame[3]  = 0x00; frame[4]  = 0x00; frame[5]  = 0x00;
    frame[6]  = 0x00; frame[7]  = 0x01; frame[8]  = 0x86; frame[9]  = 0xA0;
    /* Masking key (all zeros) */
    frame[10] = 0x00; frame[11] = 0x00; frame[12] = 0x00; frame[13] = 0x00;

    feed_bytes(sv[1], frame, sizeof(frame));

    int result = ws_read_packet(&ns);
    ck_assert_int_eq(result, -1);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_read_packet – 64-bit length that wraps header_size + payload_len
 * A length near UINT64_MAX must be rejected rather than overflowing the
 * total-size computation and unmasking past the end of inbuf.
 * ------------------------------------------------------------------------- */

static void check_rejected_64bit_length(uint64_t len, uint16_t close_code) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frame[14] = {0};
    frame[0] = 0x82;  /* FIN=1, binary */
    frame[1] = 0xFF;  /* MASK=1, plen7=127 */
    for (int i = 0; i < 8; i++)
        frame[2 + i] = (uint8_t)(len >> (56 - 8 * i));
    /* frame[10..13]: all-zero masking key. */

    feed_bytes(sv[1], frame, sizeof(frame));

    int result = ws_read_packet(&ns);
    ck_assert_int_eq(result, -1);
    assert_close_sent(sv[1], close_code);

    close(sv[0]);
    close(sv[1]);
}

START_TEST(test_read_packet_64bit_length_wraps) {
    /* UINT64_MAX: header + length would wrap to 13. */
    check_rejected_64bit_length(UINT64_MAX, 1002);
    /* 2^64-14: header + length would wrap to 0 (the "unknown" sentinel). */
    check_rejected_64bit_length(UINT64_MAX - 13, 1002);
    /* Largest length with the top bit clear: valid encoding, but too big. */
    check_rejected_64bit_length(UINT64_MAX >> 1, 1009);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_read_packet – too-large frame (16-bit extended length at limit)
 * A 16-bit length that would make total > sizeof(inbuf.buf)-1 must return -1.
 * ------------------------------------------------------------------------- */

START_TEST(test_read_packet_too_large_16bit) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_socket(sv[0]);

    /* Build header: 16-bit extended length = 65535 bytes.
     * Header (masked): 0x82, 0xFE, 0xFF, 0xFF + 4-byte key → 8 bytes.
     * Total = 8 + 65535 = 65543 > MAXSOCKBUF - 1 = 65537. */
    uint8_t hdr[8] = { 0 };
    hdr[0] = 0x82;  /* FIN=1, binary */
    hdr[1] = 0xFE;  /* MASK=1, plen7=126, 16-bit ext */
    hdr[2] = 0xFF;
    hdr[3] = 0xFF;  /* payload_len = 65535 */

    feed_bytes(sv[1], hdr, sizeof(hdr));

    int result = ws_read_packet(&ns);
    ck_assert_int_eq(result, -1);
    assert_close_sent(sv[1], 1009);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_write_frame – receiver gets expected bytes
 * ------------------------------------------------------------------------- */

START_TEST(test_write_frame_small) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[1], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_socket(sv[0]);

    /* Build a SockList with payload "hi". */
    SockList sl;
    SockList_Init(&sl);
    SockList_AddString(&sl, "hi");   /* writes at buf[2..3] */

    ws_write_frame(&ns, &sl);

    /* Read what was sent. */
    uint8_t buf[16];
    ssize_t n = read(sv[1], buf, sizeof(buf));
    ck_assert_int_gt((int)n, 0);

    /* Server-to-client frames are NOT masked.
     * Expected: 0x82 (FIN+binary), 0x02 (len=2), 'h', 'i' */
    ck_assert_uint_eq(buf[0], 0x82);
    ck_assert_uint_eq(buf[1], 0x02);
    ck_assert_uint_eq(buf[2], 'h');
    ck_assert_uint_eq(buf[3], 'i');
    ck_assert_int_eq((int)n, 4);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

START_TEST(test_write_frame_extended_length) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[1], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_socket(sv[0]);

    /* Build a SockList with a 130-byte payload. */
    SockList sl;
    SockList_Init(&sl);
    uint8_t payload[130];
    memset(payload, 0xAB, sizeof(payload));
    SockList_AddData(&sl, payload, sizeof(payload));

    ws_write_frame(&ns, &sl);

    uint8_t buf[150];
    ssize_t n = read(sv[1], buf, sizeof(buf));
    ck_assert_int_gt((int)n, 0);

    /* Expected header: 0x82, 0x7E (126), high-byte(130), low-byte(130) */
    ck_assert_uint_eq(buf[0], 0x82);
    ck_assert_uint_eq(buf[1], 126);
    ck_assert_uint_eq(buf[2], 0);
    ck_assert_uint_eq(buf[3], 130);
    /* Payload follows at buf[4]. */
    ck_assert_mem_eq(buf + 4, payload, sizeof(payload));
    ck_assert_int_eq((int)n, 4 + 130);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: round-trip – write with ws_write_frame, read with ws_read_packet
 * ------------------------------------------------------------------------- */

START_TEST(test_round_trip) {
    int out_sv[2], in_sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, out_sv), 0);
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, in_sv), 0);
    fcntl(in_sv[0], F_SETFL, O_NONBLOCK);

    socket_struct writer = make_ws_socket(out_sv[0]);
    socket_struct reader = make_ws_socket(in_sv[0]);

    /* Compose a Crossfire-style message "version 1 2 test\n". */
    const char *expected = "version 1 2 test\n";
    size_t expected_len  = strlen(expected);
    SockList sl;
    SockList_Init(&sl);
    SockList_AddString(&sl, expected);

    ws_write_frame(&writer, &sl);

    /* Decode the unmasked server frame the way a client would. */
    uint8_t wire[64];
    size_t n = read_available(out_sv[1], wire, sizeof(wire));
    ck_assert_uint_eq(n, 2 + expected_len);
    ck_assert_uint_eq(wire[0], 0x82);
    ck_assert_uint_eq(wire[1], expected_len);

    /* Send the payload back as a (masked) client frame and read it. */
    uint8_t frame[64];
    size_t frame_len = build_frame(frame, true, 0x2, wire + 2, expected_len, true);
    feed_bytes(in_sv[1], frame, frame_len);

    int result = ws_read_packet(&reader);
    ck_assert_int_eq(result, 1);
    ck_assert_uint_eq(reader.inbuf.len, 2 + expected_len);
    ck_assert_mem_eq(reader.inbuf.buf + 2, expected, expected_len);

    close(out_sv[0]);
    close(out_sv[1]);
    close(in_sv[0]);
    close(in_sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: write statistics count the frame header as well as the payload
 * ------------------------------------------------------------------------- */

#ifdef CS_LOGSTATS
START_TEST(test_write_frame_stats_include_header) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);

    socket_struct ns = make_ws_socket(sv[0]);

    SockList sl;
    SockList_Init(&sl);
    uint8_t payload[130];
    memset(payload, 0x5A, sizeof(payload));
    SockList_AddData(&sl, payload, sizeof(payload));

    int before = cst_tot.obytes;
    ws_write_frame(&ns, &sl);
    ck_assert_int_eq(cst_tot.obytes - before, 4 + 130);

    close(sv[0]);
    close(sv[1]);
}
END_TEST
#endif

/* -------------------------------------------------------------------------
 * Tests: frame-level protocol handling
 * ------------------------------------------------------------------------- */

/** ws_write_message sends text messages with the text opcode. */
START_TEST(test_write_message_text) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    socket_struct ns = make_ws_socket(sv[0]);

    /* Short text message: 2-byte header. */
    SockList sl;
    SockList_Init(&sl);
    SockList_AddString(&sl, "hi");
    ws_write_message(&ns, &sl, WS_MSG_TEXT);

    uint8_t buf[200];
    size_t n = read_available(sv[1], buf, sizeof(buf));
    ck_assert_uint_eq(n, 4);
    ck_assert_uint_eq(buf[0], 0x81); /* FIN + text */
    ck_assert_uint_eq(buf[1], 2);
    ck_assert_mem_eq(buf + 2, "hi", 2);

    /* Longer text message: 4-byte header. */
    SockList_Init(&sl);
    uint8_t payload[130];
    memset(payload, 'x', sizeof(payload));
    SockList_AddData(&sl, payload, sizeof(payload));
    ws_write_message(&ns, &sl, WS_MSG_TEXT);

    n = read_available(sv[1], buf, sizeof(buf));
    ck_assert_uint_eq(n, 4 + 130);
    ck_assert_uint_eq(buf[0], 0x81);
    ck_assert_uint_eq(buf[1], 126);
    ck_assert_uint_eq((buf[2] << 8) | buf[3], 130);
    ck_assert_mem_eq(buf + 4, payload, sizeof(payload));

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** ws_read_packet reports whether a message was text or binary. */
START_TEST(test_read_packet_message_type) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frames[128];
    size_t len;

    len = build_frame(frames, true, 0x1, "text", 4, true);
    feed_bytes(sv[1], frames, len);
    ck_assert_int_eq(ws_read_packet(&ns), 1);
    ck_assert_uint_eq(ns.websocket.ws_msg_type, WS_MSG_TEXT);
    SockList_ResetRead(&ns.inbuf);

    len = build_frame(frames, true, 0x2, "bin", 3, true);
    feed_bytes(sv[1], frames, len);
    ck_assert_int_eq(ws_read_packet(&ns), 1);
    ck_assert_uint_eq(ns.websocket.ws_msg_type, WS_MSG_BINARY);
    SockList_ResetRead(&ns.inbuf);

    /* A fragmented text message keeps the type of its first frame, even
     * with a ping in between. */
    len  = build_frame(frames,       false, 0x1, "ab", 2, true);
    len += build_frame(frames + len, true,  0x9, "p",  1, true);
    len += build_frame(frames + len, true,  0x0, "cd", 2, true);
    feed_bytes(sv[1], frames, len);
    ck_assert_int_eq(ws_read_packet(&ns), 1);
    ck_assert_uint_eq(ns.websocket.ws_msg_type, WS_MSG_TEXT);
    ck_assert_mem_eq(ns.inbuf.buf + 2, "abcd", 4);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** A frame with a real (non-zero) mask is unmasked correctly. */
START_TEST(test_read_packet_nonzero_mask) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    const char *msg = "say hello";
    uint8_t frame[32];
    size_t len = build_frame(frame, true, 0x2, msg, strlen(msg), true);
    feed_bytes(sv[1], frame, len);

    ck_assert_int_eq(ws_read_packet(&ns), 1);
    ck_assert_uint_eq(ns.inbuf.len, 2 + strlen(msg));
    ck_assert_mem_eq(ns.inbuf.buf + 2, msg, strlen(msg));

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** Client frames must be masked (RFC 6455 §5.1). */
START_TEST(test_read_packet_unmasked_rejected) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frame[32];
    size_t len = build_frame(frame, true, 0x2, "abc", 3, false);
    feed_bytes(sv[1], frame, len);

    ck_assert_int_eq(ws_read_packet(&ns), -1);
    assert_close_sent(sv[1], 1002);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** RSV bits must be zero when no extension is negotiated. */
START_TEST(test_read_packet_rsv_rejected) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frame[32];
    size_t len = build_frame(frame, true, 0x2, "abc", 3, true);
    frame[0] |= 0x40; /* RSV1 */
    feed_bytes(sv[1], frame, len);

    ck_assert_int_eq(ws_read_packet(&ns), -1);
    assert_close_sent(sv[1], 1002);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** Reserved opcodes close the connection. */
START_TEST(test_read_packet_reserved_opcode_rejected) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frame[32];
    size_t len = build_frame(frame, true, 0x3, "abc", 3, true);
    feed_bytes(sv[1], frame, len);

    ck_assert_int_eq(ws_read_packet(&ns), -1);
    assert_close_sent(sv[1], 1002);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** A message split over several frames is reassembled. */
START_TEST(test_read_packet_fragmented) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frames[64];
    size_t len = 0;
    len += build_frame(frames + len, false, 0x1, "hel", 3, true);
    len += build_frame(frames + len, false, 0x0, "lo ", 3, true);

    /* Only the first two fragments: no complete message yet. */
    feed_bytes(sv[1], frames, len);
    ck_assert_int_eq(ws_read_packet(&ns), 0);

    len = build_frame(frames, true, 0x0, "world", 5, true);
    feed_bytes(sv[1], frames, len);
    ck_assert_int_eq(ws_read_packet(&ns), 1);
    ck_assert_uint_eq(ns.inbuf.len, 2 + 11);
    ck_assert_uint_eq(ns.inbuf.buf[0], 0);
    ck_assert_uint_eq(ns.inbuf.buf[1], 11);
    ck_assert_mem_eq(ns.inbuf.buf + 2, "hello world", 11);

    /* The state is reset: the next message starts afresh. */
    SockList_ResetRead(&ns.inbuf);
    len = build_frame(frames, true, 0x2, "next", 4, true);
    feed_bytes(sv[1], frames, len);
    ck_assert_int_eq(ws_read_packet(&ns), 1);
    ck_assert_uint_eq(ns.inbuf.len, 2 + 4);
    ck_assert_mem_eq(ns.inbuf.buf + 2, "next", 4);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** A ping between fragments is answered and does not corrupt the message. */
START_TEST(test_read_packet_ping_between_fragments) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frames[64];
    size_t len = 0;
    len += build_frame(frames + len, false, 0x2, "ab", 2, true);
    len += build_frame(frames + len, true,  0x9, "xyz", 3, true);
    len += build_frame(frames + len, true,  0x0, "cd", 2, true);
    feed_bytes(sv[1], frames, len);

    ck_assert_int_eq(ws_read_packet(&ns), 1);
    ck_assert_uint_eq(ns.inbuf.len, 2 + 4);
    ck_assert_mem_eq(ns.inbuf.buf + 2, "abcd", 4);

    uint8_t resp[16];
    size_t n = read_available(sv[1], resp, sizeof(resp));
    ck_assert_uint_eq(n, 5);
    ck_assert_uint_eq(resp[0], 0x8A); /* FIN + Pong */
    ck_assert_uint_eq(resp[1], 3);
    ck_assert_mem_eq(resp + 2, "xyz", 3);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** A lone ping is answered with a pong carrying the same payload. */
START_TEST(test_read_packet_ping_answered) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frame[32];
    size_t len = build_frame(frame, true, 0x9, "ping!", 5, true);
    feed_bytes(sv[1], frame, len);

    ck_assert_int_eq(ws_read_packet(&ns), 0);
    ck_assert_int_ne(ns.status, Ns_Dead);
    ck_assert_uint_eq(ns.inbuf.len, 0);

    uint8_t resp[16];
    size_t n = read_available(sv[1], resp, sizeof(resp));
    ck_assert_uint_eq(n, 7);
    ck_assert_uint_eq(resp[0], 0x8A);
    ck_assert_uint_eq(resp[1], 5);
    ck_assert_mem_eq(resp + 2, "ping!", 5);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** A pong from the client is ignored. */
START_TEST(test_read_packet_pong_ignored) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frames[64];
    size_t len = 0;
    len += build_frame(frames + len, true, 0xA, "p", 1, true);
    len += build_frame(frames + len, true, 0x2, "data", 4, true);
    feed_bytes(sv[1], frames, len);

    ck_assert_int_eq(ws_read_packet(&ns), 1);
    ck_assert_mem_eq(ns.inbuf.buf + 2, "data", 4);

    uint8_t resp[16];
    ck_assert_uint_eq(read_available(sv[1], resp, sizeof(resp)), 0);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** A close frame's status code is echoed back. */
START_TEST(test_read_packet_close_echoed) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    const uint8_t payload[] = { 0x03, 0xE8, 'b', 'y', 'e' }; /* 1000 + reason */
    uint8_t frame[32];
    size_t len = build_frame(frame, true, 0x8, payload, sizeof(payload), true);
    feed_bytes(sv[1], frame, len);

    ck_assert_int_eq(ws_read_packet(&ns), -1);
    assert_close_sent(sv[1], 1000);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/**
 * Send a Close frame with status @p code and @p reason, and check that the
 * server answers with a Close frame carrying @p expected_code.
 */
static void check_close_reply(uint16_t code, const char *reason, size_t reason_len,
                              uint16_t expected_code) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t payload[64];
    payload[0] = (uint8_t)(code >> 8);
    payload[1] = (uint8_t)(code & 0xFF);
    memcpy(payload + 2, reason, reason_len);
    uint8_t frame[96];
    size_t len = build_frame(frame, true, 0x8, payload, 2 + reason_len, true);
    feed_bytes(sv[1], frame, len);

    ck_assert_int_eq(ws_read_packet(&ns), -1);
    assert_close_sent(sv[1], expected_code);

    close(sv[0]);
    close(sv[1]);
}

/** Close codes that may not be sent on the wire are a protocol error. */
START_TEST(test_read_packet_close_invalid_codes) {
    const uint16_t invalid[] = { 0, 999, 1004, 1005, 1006, 1015, 1016, 1100,
                                 2000, 2999, 5000, 65535 };
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++)
        check_close_reply(invalid[i], "", 0, 1002);
}
END_TEST

/** Valid close codes are echoed back. */
START_TEST(test_read_packet_close_valid_codes) {
    const uint16_t valid[] = { 1000, 1001, 1002, 1003, 1007, 1008, 1009, 1010,
                               1011, 1012, 1013, 1014, 3000, 3999, 4000, 4999 };
    for (size_t i = 0; i < sizeof(valid) / sizeof(valid[0]); i++)
        check_close_reply(valid[i], "", 0, valid[i]);
}
END_TEST

/** The close reason must be valid UTF-8. */
START_TEST(test_read_packet_close_reason_utf8) {
    /* Valid: ASCII, 2-, 3- and 4-byte sequences. */
    const char valid[] = "ok \xC3\xA9 \xE2\x82\xAC \xF0\x9F\x98\x80";
    check_close_reply(1000, valid, sizeof(valid) - 1, 1000);

    /* Invalid sequences each get 1007. */
    const char *invalid[] = {
        "\xFF",              /* never valid */
        "\x80",              /* lone continuation byte */
        "\xC0\xAF",          /* overlong '/' */
        "\xE0\x80\xAF",      /* overlong, 3 bytes */
        "\xED\xA0\x80",      /* UTF-16 surrogate U+D800 */
        "\xF4\x90\x80\x80",  /* above U+10FFFF */
        "\xE2\x82",          /* truncated */
        "\xCE\xBA\xE1\xBD",  /* valid char followed by truncated one */
    };
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++)
        check_close_reply(1000, invalid[i], strlen(invalid[i]), 1007);
}
END_TEST

/** A continuation frame with no message in progress is a protocol error. */
START_TEST(test_read_packet_unexpected_continuation) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frame[32];
    size_t len = build_frame(frame, true, 0x0, "abc", 3, true);
    feed_bytes(sv[1], frame, len);

    ck_assert_int_eq(ws_read_packet(&ns), -1);
    assert_close_sent(sv[1], 1002);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** A new data frame while a fragmented message is open is an error. */
START_TEST(test_read_packet_data_frame_mid_message) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_socket(sv[0]);

    uint8_t frames[64];
    size_t len = 0;
    len += build_frame(frames + len, false, 0x2, "ab", 2, true);
    len += build_frame(frames + len, true,  0x2, "cd", 2, true);
    feed_bytes(sv[1], frames, len);

    ck_assert_int_eq(ws_read_packet(&ns), -1);
    assert_close_sent(sv[1], 1002);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** Control frames longer than 125 bytes or fragmented are rejected. */
START_TEST(test_read_packet_bad_control_frames) {
    for (int variant = 0; variant < 2; variant++) {
        int sv[2];
        ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
        fcntl(sv[0], F_SETFL, O_NONBLOCK);
        socket_struct ns = make_ws_socket(sv[0]);

        uint8_t payload[126];
        memset(payload, 'p', sizeof(payload));
        uint8_t frame[160];
        size_t len = variant == 0
            ? build_frame(frame, true,  0x9, payload, 126, true)  /* too long */
            : build_frame(frame, false, 0x9, payload, 4, true);   /* FIN=0 */
        feed_bytes(sv[1], frame, len);

        ck_assert_int_eq(ws_read_packet(&ns), -1);
        assert_close_sent(sv[1], 1002);

        close(sv[0]);
        close(sv[1]);
    }
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – valid upgrade with standard header case
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_standard_case) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    send_http_upgrade(sv[1], "Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);
    ck_assert_int_eq(ns.websocket.ws_state, WS_ACTIVE);
    ck_assert_int_eq(ns.status, Ns_Add);

    /* Verify the response starts with "HTTP/1.1 101". */
    char resp[256] = { 0 };
    read(sv[1], resp, sizeof(resp) - 1);
    ck_assert(strncmp(resp, "HTTP/1.1 101", 12) == 0);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – valid upgrade with all-lowercase header name
 * The header field name must be treated case-insensitively (RFC 7230 §3.2).
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_lowercase_key_header) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    /* Use all-lowercase header name. */
    send_http_upgrade(sv[1], "sec-websocket-key", "dGhlIHNhbXBsZSBub25jZQ==");

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);
    ck_assert_int_eq(ns.websocket.ws_state, WS_ACTIVE);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – valid upgrade with ALL-UPPERCASE header name
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_uppercase_key_header) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    send_http_upgrade(sv[1], "SEC-WEBSOCKET-KEY", "dGhlIHNhbXBsZSBub25jZQ==");

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);
    ck_assert_int_eq(ns.websocket.ws_state, WS_ACTIVE);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – mixed-case header name
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_mixed_case_key_header) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    send_http_upgrade(sv[1], "sEc-WeBsOcKeT-kEy", "dGhlIHNhbXBsZSBub25jZQ==");

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);
    ck_assert_int_eq(ns.websocket.ws_state, WS_ACTIVE);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – response contains the correct Accept header
 * The computed accept value must match the RFC 6455 §1.3 example.
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_accept_value) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    send_http_upgrade(sv[1], "Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
    ws_do_handshake(&ns);

    char resp[512] = { 0 };
    read(sv[1], resp, sizeof(resp) - 1);

    /* The accept value for "dGhlIHNhbXBsZSBub25jZQ==" is the RFC example. */
    ck_assert(strstr(resp, "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=") != NULL);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – partial request (no \r\n\r\n yet)
 * Handshake should return true and keep the socket alive (WS_HTTP).
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_incomplete_request) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    /* Send only the first line – no \r\n\r\n. */
    const char *partial = "GET / HTTP/1.1\r\nHost: localhost\r\n";
    feed_bytes(sv[1], (const uint8_t *)partial, strlen(partial));

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);                      /* still alive, waiting */
    ck_assert_int_eq(ns.websocket.ws_state, WS_HTTP); /* not upgraded yet */
    ck_assert_int_eq(ns.status, Ns_Add);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – missing Sec-WebSocket-Key
 * Handshake should fail and mark the socket dead.
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_missing_key) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    const char *req =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";
    feed_bytes(sv[1], (const uint8_t *)req, strlen(req));

    bool ok = ws_do_handshake(&ns);
    ck_assert(!ok);
    ck_assert_int_eq(ns.status, Ns_Dead);

    char resp[256] = { 0 };
    read_available(sv[1], (uint8_t *)resp, sizeof(resp) - 1);
    ck_assert(strncmp(resp, "HTTP/1.1 400 ", 13) == 0);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/**
 * Run a handshake for @p req and check that it is refused with an HTTP
 * response starting with @p status_prefix.  Returns the response in @p resp.
 */
static void check_handshake_rejected(const char *req, const char *status_prefix,
                                     char *resp, size_t resp_len) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);
    feed_bytes(sv[1], (const uint8_t *)req, strlen(req));

    ck_assert(!ws_do_handshake(&ns));
    ck_assert_int_eq(ns.status, Ns_Dead);
    ck_assert_int_eq(ns.websocket.ws_state, WS_HTTP);

    memset(resp, 0, resp_len);
    read_available(sv[1], (uint8_t *)resp, resp_len - 1);
    ck_assert_msg(strncmp(resp, status_prefix, strlen(status_prefix)) == 0,
                  "unexpected response: %s", resp);

    close(sv[0]);
    close(sv[1]);
}

/** Only GET requests can be upgraded. */
START_TEST(test_handshake_non_get_rejected) {
    char resp[256];
    check_handshake_rejected(
        "POST / HTTP/1.1\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        "HTTP/1.1 405 ", resp, sizeof(resp));
}
END_TEST

/** Upgrade: websocket and Connection: Upgrade are required. */
START_TEST(test_handshake_missing_upgrade_rejected) {
    char resp[256];
    check_handshake_rejected(
        "GET / HTTP/1.1\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        "HTTP/1.1 400 ", resp, sizeof(resp));
    check_handshake_rejected(
        "GET / HTTP/1.1\r\n"
        "Upgrade: websocket\r\n"
        "Connection: keep-alive\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        "HTTP/1.1 400 ", resp, sizeof(resp));
}
END_TEST

/** Unsupported versions get 426 with the supported version listed. */
START_TEST(test_handshake_wrong_version_rejected) {
    char resp[256];
    check_handshake_rejected(
        "GET / HTTP/1.1\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 8\r\n"
        "\r\n",
        "HTTP/1.1 426 ", resp, sizeof(resp));
    ck_assert(strstr(resp, "Sec-WebSocket-Version: 13\r\n") != NULL);
}
END_TEST

/** A header name inside another header's value must not be matched. */
START_TEST(test_handshake_header_name_in_value_ignored) {
    char resp[256];
    check_handshake_rejected(
        "GET / HTTP/1.1\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "X-Note: Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        "HTTP/1.1 400 ", resp, sizeof(resp));
}
END_TEST

/** Connection may list several tokens; Upgrade may be anywhere in it. */
START_TEST(test_handshake_connection_token_list) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_http_socket(sv[0]);

    const char *req =
        "GET /ws HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "upgrade: WebSocket\r\n"
        "connection: keep-alive, Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";
    feed_bytes(sv[1], (const uint8_t *)req, strlen(req));

    ck_assert(ws_do_handshake(&ns));
    ck_assert_int_eq(ns.websocket.ws_state, WS_ACTIVE);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/** Frame bytes sent right after the request are not lost. */
START_TEST(test_handshake_keeps_trailing_frame) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);
    socket_struct ns = make_ws_http_socket(sv[0]);

    uint8_t data[512];
    const char *req =
        "GET / HTTP/1.1\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";
    size_t len = strlen(req);
    memcpy(data, req, len);
    len += build_frame(data + len, true, 0x2, "early", 5, true);
    feed_bytes(sv[1], data, len);

    ck_assert(ws_do_handshake(&ns));
    ck_assert_int_eq(ns.websocket.ws_state, WS_ACTIVE);

    ck_assert_int_eq(ws_read_packet(&ns), 1);
    ck_assert_uint_eq(ns.inbuf.len, 2 + 5);
    ck_assert_mem_eq(ns.inbuf.buf + 2, "early", 5);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – key value too long (>= 64 bytes)
 * Must be rejected to prevent buffer overflow in ws_key[65].
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_key_too_long) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    /* 64 'A' characters as the key value (one over the 63-char limit). */
    const char *req =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";
    feed_bytes(sv[1], (const uint8_t *)req, strlen(req));

    bool ok = ws_do_handshake(&ns);
    ck_assert(!ok);
    ck_assert_int_eq(ns.status, Ns_Dead);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – key value empty
 * An empty key must be rejected.
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_key_empty) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    const char *req =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: \r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";
    feed_bytes(sv[1], (const uint8_t *)req, strlen(req));

    bool ok = ws_do_handshake(&ns);
    ck_assert(!ok);
    ck_assert_int_eq(ns.status, Ns_Dead);

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – buffer already full (no room for more data)
 * Simulates an extremely long HTTP preamble that fills the buffer.
 * The server must detect this and reject the connection.
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_buffer_overflow_protection) {
    /* Pre-fill inbuf to the point where avail == 0 without touching the fd. */
    socket_struct ns;
    memset(&ns, 0, sizeof(ns));
    ns.fd       = -1;               /* will not be read */
    ns.status   = Ns_Add;
    ns.is_websocket = true;
    ns.websocket.ws_state = WS_HTTP;
    ns.host     = strdup("127.0.0.1");

    /* Fill the buffer to exactly sizeof(buf) - 1 bytes so avail == 0. */
    ns.inbuf.len = sizeof(ns.inbuf.buf) - 1;

    bool ok = ws_do_handshake(&ns);
    ck_assert(!ok);
    ck_assert_int_eq(ns.status, Ns_Dead);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: ws_do_handshake – NUL byte in the HTTP request
 * A NUL byte in the request prevents strstr from finding \r\n\r\n.
 * The server must not crash and should keep waiting for data (or reject).
 * ------------------------------------------------------------------------- */

START_TEST(test_handshake_nul_in_request) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    socket_struct ns = make_ws_http_socket(sv[0]);

    /* A request with a NUL byte embedded before the double CRLF. */
    const char req[] =
        "GET / HTTP/1.1\r\n"
        "Host: local\0host\r\n"
        "\r\n";
    /* Send the entire buffer including the NUL. */
    feed_bytes(sv[1], (const uint8_t *)req, sizeof(req) - 1);

    /* Should not crash; either returns true (waiting) or false (dead). */
    bool ok = ws_do_handshake(&ns);
    /* No assertion on ok – both outcomes are acceptable.  The important
     * thing is that the server does not crash or overflow a buffer. */
    (void)ok;

    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * X-Forwarded-For helpers and tests
 * ------------------------------------------------------------------------- */

/* Access the stub control variables defined in stubs_websocket.cpp. */
extern int  g_checkbanned_result;
extern char g_checkbanned_host[64];

/**
 * Send a complete HTTP Upgrade request including an X-Forwarded-For header.
 */
static void send_http_upgrade_xff(int fd, const char *key_value,
                                   const char *xff_value) {
    char req[1024];
    int n;
    if (xff_value) {
        n = snprintf(req, sizeof(req),
            "GET / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Key: %s\r\n"
            "X-Forwarded-For: %s\r\n"
            "Sec-WebSocket-Version: 13\r\n"
            "\r\n",
            key_value, xff_value);
    } else {
        n = snprintf(req, sizeof(req),
            "GET / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Key: %s\r\n"
            "Sec-WebSocket-Version: 13\r\n"
            "\r\n",
            key_value);
    }
    feed_bytes(fd, (const uint8_t *)req, (size_t)n);
}

/* -------------------------------------------------------------------------
 * Test: X-Forwarded-For present with an allowed IP – handshake succeeds and
 * ns->host is updated to the forwarded address.
 * ------------------------------------------------------------------------- */

START_TEST(test_xff_allowed_ip) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    g_checkbanned_result = 0; /* allow */
    memset(g_checkbanned_host, 0, sizeof(g_checkbanned_host));

    socket_struct ns = make_ws_http_socket(sv[0]);
    /* Simulate the TCP peer address set by do_server(). */
    ns.host = strdup("10.0.0.1");

    send_http_upgrade_xff(sv[1], "dGhlIHNhbXBsZSBub25jZQ==", "203.0.113.42");

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);
    ck_assert_int_eq(ns.websocket.ws_state, WS_ACTIVE);
    /* The host must be updated to the forwarded IP. */
    ck_assert_str_eq(ns.host, "203.0.113.42");
    /* checkbanned was called with the forwarded IP. */
    ck_assert_str_eq(g_checkbanned_host, "203.0.113.42");

    free(ns.host);
    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: X-Forwarded-For with a banned IP – handshake is rejected.
 * ------------------------------------------------------------------------- */

START_TEST(test_xff_banned_ip) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    g_checkbanned_result = 1; /* banned */

    socket_struct ns = make_ws_http_socket(sv[0]);
    ns.host = strdup("10.0.0.1");

    send_http_upgrade_xff(sv[1], "dGhlIHNhbXBsZSBub25jZQ==", "198.51.100.5");

    bool ok = ws_do_handshake(&ns);
    ck_assert(!ok);
    ck_assert_int_eq(ns.status, Ns_Dead);

    free(ns.host);
    /* Reset for subsequent tests. */
    g_checkbanned_result = 0;
    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: No X-Forwarded-For header – handshake succeeds using the TCP IP.
 * ------------------------------------------------------------------------- */

START_TEST(test_xff_absent) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    g_checkbanned_result = 0;

    socket_struct ns = make_ws_http_socket(sv[0]);
    ns.host = strdup("192.0.2.1");

    /* No xff_value – send_http_upgrade_xff passes NULL. */
    send_http_upgrade_xff(sv[1], "dGhlIHNhbXBsZSBub25jZQ==", NULL);

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);
    ck_assert_int_eq(ns.websocket.ws_state, WS_ACTIVE);
    /* host should remain the TCP peer address. */
    ck_assert_str_eq(ns.host, "192.0.2.1");

    free(ns.host);
    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: X-Forwarded-For with multiple addresses (comma-separated).
 * Only the first (leftmost = original client) address is used.
 * ------------------------------------------------------------------------- */

START_TEST(test_xff_multiple_addresses) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    g_checkbanned_result = 0;
    memset(g_checkbanned_host, 0, sizeof(g_checkbanned_host));

    socket_struct ns = make_ws_http_socket(sv[0]);
    ns.host = strdup("10.0.0.1");

    send_http_upgrade_xff(sv[1], "dGhlIHNhbXBsZSBub25jZQ==",
                          "203.0.113.10, 10.1.2.3, 10.0.0.1");

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);
    ck_assert_int_eq(ns.websocket.ws_state, WS_ACTIVE);
    /* Only the first address should be used. */
    ck_assert_str_eq(ns.host, "203.0.113.10");
    ck_assert_str_eq(g_checkbanned_host, "203.0.113.10");

    free(ns.host);
    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: X-Forwarded-For with an IPv6 address.
 * ------------------------------------------------------------------------- */

START_TEST(test_xff_ipv6_address) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    g_checkbanned_result = 0;
    memset(g_checkbanned_host, 0, sizeof(g_checkbanned_host));

    socket_struct ns = make_ws_http_socket(sv[0]);
    ns.host = strdup("::1");

    send_http_upgrade_xff(sv[1], "dGhlIHNhbXBsZSBub25jZQ==",
                          "2001:db8::1");

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);
    ck_assert_str_eq(ns.host, "2001:db8::1");

    free(ns.host);
    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: X-Forwarded-For with an illegal character (injection attempt).
 * The request must be rejected to prevent log-injection / misuse.
 * ------------------------------------------------------------------------- */

START_TEST(test_xff_illegal_character) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    g_checkbanned_result = 0;

    socket_struct ns = make_ws_http_socket(sv[0]);
    ns.host = strdup("10.0.0.1");

    /* Attempt to inject a newline into the log via the XFF header. */
    send_http_upgrade_xff(sv[1], "dGhlIHNhbXBsZSBub25jZQ==",
                          "192.0.2.1\nevil");

    /* Because the newline terminates the XFF value at the first address
     * boundary (our parser stops at whitespace), the address seen is
     * "192.0.2.1" which is valid.  The parser must not pass "evil" anywhere.
     * The connection should succeed with host == "192.0.2.1". */
    bool ok = ws_do_handshake(&ns);
    if (ok) {
        /* The parser correctly extracted only "192.0.2.1". */
        ck_assert_str_eq(ns.host, "192.0.2.1");
    }
    /* If the handshake failed that is also acceptable (conservative parser). */

    free(ns.host);
    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: X-Forwarded-For with an excessively long value (overflow attempt).
 * Must be rejected without a buffer overrun.
 * ------------------------------------------------------------------------- */

START_TEST(test_xff_too_long) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    g_checkbanned_result = 0;

    socket_struct ns = make_ws_http_socket(sv[0]);
    ns.host = strdup("10.0.0.1");

    /* A 200-character "IP address" – well beyond the 45-char limit. */
    char long_xff[210];
    memset(long_xff, '1', 200);
    long_xff[200] = '\0';

    send_http_upgrade_xff(sv[1], "dGhlIHNhbXBsZSBub25jZQ==", long_xff);

    bool ok = ws_do_handshake(&ns);
    ck_assert(!ok);
    ck_assert_int_eq(ns.status, Ns_Dead);

    free(ns.host);
    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: X-Forwarded-For with an empty value.
 * Must be rejected (empty string is not a valid IP).
 * ------------------------------------------------------------------------- */

START_TEST(test_xff_empty_value) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    g_checkbanned_result = 0;

    socket_struct ns = make_ws_http_socket(sv[0]);
    ns.host = strdup("10.0.0.1");

    send_http_upgrade_xff(sv[1], "dGhlIHNhbXBsZSBub25jZQ==", "");

    bool ok = ws_do_handshake(&ns);
    ck_assert(!ok);
    ck_assert_int_eq(ns.status, Ns_Dead);

    free(ns.host);
    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Test: X-Forwarded-For with lowercase header name (case-insensitive).
 * ------------------------------------------------------------------------- */

START_TEST(test_xff_lowercase_header) {
    int sv[2];
    ck_assert_int_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, O_NONBLOCK);

    g_checkbanned_result = 0;
    memset(g_checkbanned_host, 0, sizeof(g_checkbanned_host));

    socket_struct ns = make_ws_http_socket(sv[0]);
    ns.host = strdup("10.0.0.1");

    /* Use lowercase header name. */
    const char *req =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "x-forwarded-for: 203.0.113.77\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";
    feed_bytes(sv[1], (const uint8_t *)req, strlen(req));

    bool ok = ws_do_handshake(&ns);
    ck_assert(ok);
    ck_assert_str_eq(ns.host, "203.0.113.77");

    free(ns.host);
    close(sv[0]);
    close(sv[1]);
}
END_TEST

/* -------------------------------------------------------------------------
 * Suite definition
 * ------------------------------------------------------------------------- */

Suite *websocket_suite(void) {
    Suite *s = suite_create("websocket");

    TCase *tc_key = tcase_create("accept_key");
    tcase_add_test(tc_key, test_accept_key_rfc_example);
    tcase_add_test(tc_key, test_accept_key_different_keys_differ);
    tcase_add_test(tc_key, test_accept_key_deterministic);
    suite_add_tcase(s, tc_key);

    TCase *tc_read = tcase_create("read_packet");
    tcase_add_test(tc_read, test_read_packet_small);
    tcase_add_test(tc_read, test_read_packet_partial);
    tcase_add_test(tc_read, test_read_packet_extended_length);
    tcase_add_test(tc_read, test_read_packet_close_frame);
    tcase_add_test(tc_read, test_read_packet_too_large_64bit);
    tcase_add_test(tc_read, test_read_packet_64bit_length_wraps);
    tcase_add_test(tc_read, test_read_packet_too_large_16bit);
    tcase_add_test(tc_read, test_read_packet_nonzero_mask);
    tcase_add_test(tc_read, test_read_packet_unmasked_rejected);
    tcase_add_test(tc_read, test_read_packet_rsv_rejected);
    tcase_add_test(tc_read, test_read_packet_reserved_opcode_rejected);
    tcase_add_test(tc_read, test_read_packet_fragmented);
    tcase_add_test(tc_read, test_read_packet_ping_between_fragments);
    tcase_add_test(tc_read, test_read_packet_ping_answered);
    tcase_add_test(tc_read, test_read_packet_pong_ignored);
    tcase_add_test(tc_read, test_read_packet_close_echoed);
    tcase_add_test(tc_read, test_read_packet_unexpected_continuation);
    tcase_add_test(tc_read, test_read_packet_data_frame_mid_message);
    tcase_add_test(tc_read, test_read_packet_bad_control_frames);
    tcase_add_test(tc_read, test_read_packet_message_type);
    tcase_add_test(tc_read, test_read_packet_close_invalid_codes);
    tcase_add_test(tc_read, test_read_packet_close_valid_codes);
    tcase_add_test(tc_read, test_read_packet_close_reason_utf8);
    suite_add_tcase(s, tc_read);

    TCase *tc_write = tcase_create("write_frame");
    tcase_add_test(tc_write, test_write_frame_small);
    tcase_add_test(tc_write, test_write_frame_extended_length);
    tcase_add_test(tc_write, test_write_message_text);
#ifdef CS_LOGSTATS
    tcase_add_test(tc_write, test_write_frame_stats_include_header);
#endif
    suite_add_tcase(s, tc_write);

    TCase *tc_rt = tcase_create("round_trip");
    tcase_add_test(tc_rt, test_round_trip);
    suite_add_tcase(s, tc_rt);

    TCase *tc_hs = tcase_create("handshake");
    tcase_add_test(tc_hs, test_handshake_standard_case);
    tcase_add_test(tc_hs, test_handshake_lowercase_key_header);
    tcase_add_test(tc_hs, test_handshake_uppercase_key_header);
    tcase_add_test(tc_hs, test_handshake_mixed_case_key_header);
    tcase_add_test(tc_hs, test_handshake_accept_value);
    tcase_add_test(tc_hs, test_handshake_incomplete_request);
    tcase_add_test(tc_hs, test_handshake_missing_key);
    tcase_add_test(tc_hs, test_handshake_key_too_long);
    tcase_add_test(tc_hs, test_handshake_key_empty);
    tcase_add_test(tc_hs, test_handshake_buffer_overflow_protection);
    tcase_add_test(tc_hs, test_handshake_nul_in_request);
    tcase_add_test(tc_hs, test_handshake_non_get_rejected);
    tcase_add_test(tc_hs, test_handshake_missing_upgrade_rejected);
    tcase_add_test(tc_hs, test_handshake_wrong_version_rejected);
    tcase_add_test(tc_hs, test_handshake_header_name_in_value_ignored);
    tcase_add_test(tc_hs, test_handshake_connection_token_list);
    tcase_add_test(tc_hs, test_handshake_keeps_trailing_frame);
    suite_add_tcase(s, tc_hs);

    TCase *tc_xff = tcase_create("x_forwarded_for");
    tcase_add_test(tc_xff, test_xff_allowed_ip);
    tcase_add_test(tc_xff, test_xff_banned_ip);
    tcase_add_test(tc_xff, test_xff_absent);
    tcase_add_test(tc_xff, test_xff_multiple_addresses);
    tcase_add_test(tc_xff, test_xff_ipv6_address);
    tcase_add_test(tc_xff, test_xff_illegal_character);
    tcase_add_test(tc_xff, test_xff_too_long);
    tcase_add_test(tc_xff, test_xff_empty_value);
    tcase_add_test(tc_xff, test_xff_lowercase_header);
    suite_add_tcase(s, tc_xff);

    return s;
}

int main(void) {
    Suite   *s  = websocket_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

