#ifndef BASE32_H
#define BASE32_H

#include <stddef.h>
#include <stdint.h>

// Decode Base32 string to binary
// Returns 1 on success, 0 on failure (invalid characters)
int base32_decode(const char *encoded, uint8_t *output, size_t *outlen);

// Encode binary data to Base32 string
// 'output' must have enough space (ceil(inlen*8/5) + 1)
// Null-terminates the string
void base32_encode(const uint8_t *data, size_t inlen, char *output);

#endif
