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
 * Base64 encoding (RFC 4648).
 */

#include "base64.h"

#include <limits.h>
#include <stdint.h>
#include <stddef.h>

static const char B64_CHARS[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode(const uint8_t *in, size_t in_len, char *out, size_t out_len) {
    size_t i = 0, o = 0;
    while (i < in_len) {
        size_t remaining = in_len - i;
        uint32_t b = (uint32_t)in[i++] << 16;
        if (i < in_len) b |= (uint32_t)in[i++] << 8;
        if (i < in_len) b |= in[i++];
        if ((o + 5) > out_len) {
            return -1;
        }
        out[o++] = B64_CHARS[(b >> 18) & 0x3F];
        out[o++] = B64_CHARS[(b >> 12) & 0x3F];
        out[o++] = (remaining >= 2) ? B64_CHARS[(b >> 6) & 0x3F] : '=';
        out[o++] = (remaining >= 3) ? B64_CHARS[ b       & 0x3F] : '=';
    }
    out[o] = '\0';
    if (o > (size_t)INT_MAX)
        return -1;
    return (int)o;
}
