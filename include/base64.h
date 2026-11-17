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

#include <string>

/**
 * Base64-encode @p in_len bytes from @p in.
 *
 * @param in      Input bytes.
 * @param in_len  Number of input bytes.
 * @return The encoded string.
 */
std::string base64_encode(const uint8_t *in, size_t in_len);

#endif /* BASE64_H */
