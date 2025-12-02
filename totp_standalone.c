#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "sha1.h"
#include "base32.h"

// RFC 4226 dynamic truncation
uint32_t hotp(const uint8_t *key, size_t key_len, uint64_t counter) {
    uint8_t msg[8];
    uint8_t hash[20];

    // Encode counter as big-endian
    for (int i = 7; i >= 0; i--) {
        msg[i] = counter & 0xFF;
        counter >>= 8;
    }

    // HMAC-SHA1(key, msg)
    uint8_t k_ipad[64], k_opad[64];
    uint8_t tk[20];

    // If key longer than block, hash it
    if (key_len > 64) {
        SHA1_CTX ctx;
        SHA1_Init(&ctx);
        SHA1_Update(&ctx, key, key_len);
        SHA1_Final(tk, &ctx);
        key = tk;
        key_len = 20;
    }

    memset(k_ipad, 0x36, 64);
    memset(k_opad, 0x5C, 64);

    for (size_t i = 0; i < key_len; i++) {
        k_ipad[i] ^= key[i];
        k_opad[i] ^= key[i];
    }

    // inner hash
    SHA1_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, k_ipad, 64);
    SHA1_Update(&ctx, msg, 8);
    SHA1_Final(hash, &ctx);

    // outer hash
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, k_opad, 64);
    SHA1_Update(&ctx, hash, 20);
    SHA1_Final(hash, &ctx);

    // Dynamic truncation
    int offset = hash[19] & 0x0F;
    uint32_t binary =
        ((hash[offset] & 0x7F) << 24) |
        ((hash[offset+1] & 0xFF) << 16) |
        ((hash[offset+2] & 0xFF) << 8) |
        (hash[offset+3] & 0xFF);

    return binary % 1000000; // 6 digits
}

uint32_t totp(const uint8_t *key, size_t key_len) {
    uint64_t timestep = (uint64_t)time(NULL) / 30;
    return hotp(key, key_len, timestep);
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
    uint32_t code = totp(key, key_len);

    printf("TOTP code = %06u\n", code);
    return 0;
}

