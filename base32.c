#include "base32.h"
#include <ctype.h>
#include <string.h>

// RFC 4648 Base32 alphabet
static const char base32_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

// Lookup table for decoding
// -1 = invalid
static const int8_t base32_lookup[256] = {
    [0 ... 255] = -1,  // mark everything invalid by default
    ['A']=0,['B']=1,['C']=2,['D']=3,['E']=4,['F']=5,['G']=6,['H']=7,
    ['I']=8,['J']=9,['K']=10,['L']=11,['M']=12,['N']=13,['O']=14,['P']=15,
    ['Q']=16,['R']=17,['S']=18,['T']=19,['U']=20,['V']=21,['W']=22,['X']=23,
    ['Y']=24,['Z']=25,['2']=26,['3']=27,['4']=28,['5']=29,['6']=30,['7']=31
};

int base32_decode(const char *encoded, uint8_t *output, size_t *outlen) {
    size_t len = strlen(encoded);
    size_t buffer = 0;   // bit buffer
    int bitsLeft = 0;
    size_t count = 0;

    for (size_t i = 0; i < len; i++) {
        char c = toupper((unsigned char)encoded[i]);
        if (c == '=') break; // padding

        int8_t val = base32_lookup[(unsigned char)c];
        if (val == -1) return 0; // invalid character

        buffer <<= 5;
        buffer |= val;
        bitsLeft += 5;

        if (bitsLeft >= 8) {
            output[count++] = (buffer >> (bitsLeft - 8)) & 0xFF;
            bitsLeft -= 8;
        }
    }

    *outlen = count;
    return 1;
}

void base32_encode(const uint8_t *data, size_t inlen, char *output) {
    int buffer = 0, bitsLeft = 0;
    size_t count = 0;

    for (size_t i = 0; i < inlen; i++) {
        buffer <<= 8;
        buffer |= data[i];
        bitsLeft += 8;

        while (bitsLeft >= 5) {
            int index = (buffer >> (bitsLeft - 5)) & 0x1F;
            output[count++] = base32_alphabet[index];
            bitsLeft -= 5;
        }
    }

    if (bitsLeft > 0) {
        int index = (buffer << (5 - bitsLeft)) & 0x1F;
        output[count++] = base32_alphabet[index];
    }

    output[count] = '\0'; // null terminate
}
