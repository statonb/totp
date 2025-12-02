#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include "base32.h"

// HOTP: HMAC-SHA1(secret, counter) → dynamic truncation → 6-digit code
uint32_t hotp(const uint8_t *secret, size_t secret_len, uint64_t counter)
{
    uint8_t counter_bytes[8];
    uint8_t hmac[EVP_MAX_MD_SIZE];
    unsigned int hmac_len;

    // Convert counter to big-endian 8-byte array
    for (int i = 7; i >= 0; i--) {
        counter_bytes[i] = counter & 0xFF;
        counter >>= 8;
    }

    // HMAC-SHA1(secret, counter)
    HMAC(EVP_sha1(),
         secret, secret_len,
         counter_bytes, 8,
         hmac, &hmac_len);

    // Dynamic truncation
    uint8_t offset = hmac[19] & 0x0F;

    uint32_t bin_code =
        ((hmac[offset] & 0x7F) << 24) |
        ((hmac[offset + 1] & 0xFF) << 16) |
        ((hmac[offset + 2] & 0xFF) << 8) |
        ((hmac[offset + 3] & 0xFF));

    // 6-digit OTP
    return bin_code % 1000000;
}

// TOTP: time-based HOTP
uint32_t totp(const uint8_t *secret, size_t secret_len, int timestep)
{
    time_t t = time(NULL);
    uint64_t counter = (uint64_t)(t / timestep);
    return hotp(secret, secret_len, counter);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s BASE32_SECRET\n", argv[0]);
        return 1;
    }

    const char *base32_secret = argv[1];
    uint8_t key[64];
    size_t key_len = 0;

    // Step 1: Decode Base32 string to raw binary key
    if (!base32_decode(base32_secret, key, &key_len)) {
        fprintf(stderr, "Error: invalid Base32 string.\n");
        return 1;
    }

    // Step 2: Compute TOTP using the decoded key
    uint32_t code = totp(key, key_len, 30);

    printf("TOTP code = %06u\n", code);
    return 0;
}

