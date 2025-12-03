#include "base64.h"

static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static const int8_t base64_reverse_table[256] = {
    /* Initialize all entries to -1 */
    [0 ... 255] = -1,

    /* Uppercase */
    ['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,  ['E'] = 4,  ['F'] = 5,
    ['G'] = 6,  ['H'] = 7,  ['I'] = 8,  ['J'] = 9,  ['K'] = 10, ['L'] = 11,
    ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15, ['Q'] = 16, ['R'] = 17,
    ['S'] = 18, ['T'] = 19, ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
    ['Y'] = 24, ['Z'] = 25,

    /* Lowercase */
    ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31,
    ['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35, ['k'] = 36, ['l'] = 37,
    ['m'] = 38, ['n'] = 39, ['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43,
    ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47, ['w'] = 48, ['x'] = 49,
    ['y'] = 50, ['z'] = 51,

    /* Digits */
    ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55, ['4'] = 56,
    ['5'] = 57, ['6'] = 58, ['7'] = 59, ['8'] = 60, ['9'] = 61,

    /* Symbols */
    ['+'] = 62, ['/'] = 63,
};

size_t base64_encode(const uint8_t *input, size_t input_len, char *output)
{
    size_t out_len = 0;

    for (size_t i = 0; i < input_len; i += 3) {
        uint32_t triple = input[i] << 16;

        if (i + 1 < input_len) triple |= input[i + 1] << 8;
        if (i + 2 < input_len) triple |= input[i + 2];

        output[out_len++] = base64_table[(triple >> 18) & 0x3F];
        output[out_len++] = base64_table[(triple >> 12) & 0x3F];

        if (i + 1 < input_len)
            output[out_len++] = base64_table[(triple >> 6) & 0x3F];
        else
            output[out_len++] = '=';

        if (i + 2 < input_len)
            output[out_len++] = base64_table[triple & 0x3F];
        else
            output[out_len++] = '=';
    }

    return out_len;
}

int base64_decode(const char *input, size_t input_len, uint8_t *output)
{
    if (input_len % 4 != 0)
        return -1;

    size_t out_len = 0;

    for (size_t i = 0; i < input_len; i += 4) {

        int8_t a = base64_reverse_table[(unsigned char)input[i]];
        int8_t b = base64_reverse_table[(unsigned char)input[i + 1]];

        if (a < 0 || b < 0)
            return -1;

        uint32_t triple = (a << 18) | (b << 12);

        int8_t c = base64_reverse_table[(unsigned char)input[i + 2]];
        int8_t d = base64_reverse_table[(unsigned char)input[i + 3]];

        if (input[i + 2] == '=') {
            /* Must also have '=' at position i+3 */
            if (input[i + 3] != '=')
                return -1;

            output[out_len++] = (triple >> 16) & 0xFF;
        }
        else if (input[i + 3] == '=') {
            if (c < 0)
                return -1;

            triple |= (c << 6);

            output[out_len++] = (triple >> 16) & 0xFF;
            output[out_len++] = (triple >> 8) & 0xFF;
        }
        else {
            if (c < 0 || d < 0)
                return -1;

            triple |= (c << 6) | d;

            output[out_len++] = (triple >> 16) & 0xFF;
            output[out_len++] = (triple >> 8) & 0xFF;
            output[out_len++] = triple & 0xFF;
        }
    }

    return (int)out_len;
}

