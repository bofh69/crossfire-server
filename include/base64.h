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
 * Base64 encoding (RFC 4648).
 */

#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>
#include <stddef.h>

/**
 * Base64-encode @p in_len bytes from @p in.
 *
 * Writes a NUL-terminated ASCII string to @p out.
 * @p out must be at least <tt>((in_len + 2) / 3) * 4 + 1</tt> bytes long.
 *
 * @param in      Input bytes.
 * @param in_len  Number of input bytes.
 * @param out     Output buffer.
 * @param out_len Size of output buffer.
 *
 * @return -1 in case of error, otherwise number of bytes written (including terminating NUL).
 */
ssize_t base64_encode(const uint8_t *in, size_t in_len, char *out, size_t out_len);

#endif /* BASE64_H */
