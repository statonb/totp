#include "sha1.h"
#include <string.h>

#define ROTL(a,b) (((a) << (b)) | ((a) >> (32-(b))))

static const uint32_t K[4] = {
    0x5A827999,
    0x6ED9EBA1,
    0x8F1BBCDC,
    0xCA62C1D6
};

static void SHA1_Transform(uint32_t state[5], const uint8_t buffer[64]) {
    uint32_t W[80];
    uint32_t a, b, c, d, e, t;
    int i;

    for (i = 0; i < 16; i++) {
        W[i]  = buffer[i*4] << 24;
        W[i] |= buffer[i*4+1] << 16;
        W[i] |= buffer[i*4+2] << 8;
        W[i] |= buffer[i*4+3];
    }

    for (i = 16; i < 80; i++)
        W[i] = ROTL(W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16], 1);

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    for (i = 0; i < 80; i++) {
        uint32_t f;

        if (i < 20)
            f = (b & c) | ((~b) & d);
        else if (i < 40)
            f = b ^ c ^ d;
        else if (i < 60)
            f = (b & c) | (b & d) | (c & d);
        else
            f = b ^ c ^ d;

        t = ROTL(a,5) + f + e + K[i/20] + W[i];
        e = d;
        d = c;
        c = ROTL(b,30);
        b = a;
        a = t;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

void SHA1_Init(SHA1_CTX *ctx) {
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    ctx->count = 0;
    memset(ctx->buffer, 0, 64);
}

void SHA1_Update(SHA1_CTX *ctx, const uint8_t *data, size_t len) {
    size_t i = 0;

    while (len--) {
        ctx->buffer[(ctx->count >> 3) & 63] = data[i++];
        ctx->count += 8;

        if (((ctx->count >> 3) & 63) == 0)
            SHA1_Transform(ctx->state, ctx->buffer);
    }
}

void SHA1_Final(uint8_t digest[20], SHA1_CTX *ctx) {
    uint8_t finalcount[8];
    for (int i = 0; i < 8; i++)
        finalcount[i] = (uint8_t)((ctx->count >> ((7 - i) * 8)) & 255);

    SHA1_Update(ctx, (uint8_t*)"\x80", 1);

    while ((ctx->count & 504) != 448)
        SHA1_Update(ctx, (uint8_t*)"\0", 1);

    SHA1_Update(ctx, finalcount, 8);

    for (int i = 0; i < 20; i++)
        digest[i] = (uint8_t)((ctx->state[i/4] >> ((3 - (i & 3))*8)) & 255);
}

