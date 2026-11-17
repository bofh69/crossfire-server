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
 * SHA-1 hash function (RFC 3174).
 */

#ifndef SHA1_H
#define SHA1_H

#include <stdint.h>
#include <stddef.h>

/**
 * Compute the SHA-1 digest of @p len bytes at @p data.
 *
 * @param data   Input bytes.
 * @param len    Number of input bytes.
 * @param out    Output buffer; receives exactly 20 bytes.
 */
void sha1(const uint8_t *data, size_t len, uint8_t out[20]);

#endif /* SHA1_H */
