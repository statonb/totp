#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>
#include <stdint.h>

/*
 * Encodes `input_len` bytes from `input` into Base64.
 * `output` must have enough space: at least ((input_len + 2) / 3) * 4 bytes.
 * Returns number of bytes written (not including terminating NUL).
 */
size_t base64_encode(const uint8_t *input, size_t input_len, char *output);

/*
 * Decodes a Base64 string `input` of length `input_len` into binary.
 * Returns number of bytes decoded, OR -1 on invalid Base64.
 * `output` must have enough space: at least (input_len / 4) * 3 bytes.
 */
int base64_decode(const char *input, size_t input_len, uint8_t *output);

#endif

