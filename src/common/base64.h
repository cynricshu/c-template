#include <cstdio>
#include <cstdlib>

#pragma once

namespace common {

#define B64_SYNTAX_ERROR        1
#define B64_FILE_ERROR          2
#define B64_FILE_IO_ERROR       3
#define B64_ERROR_OUT_CLOSE     4
#define B64_LINE_SIZE_TO_MIN    5
#define B64_SYNTAX_TOOMANYARGS  6

/*
** Translation Table to decode (created by author)
*/
static const char cd64[]
    = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
static void decodeblock(unsigned char *in, unsigned char *out) {
    out[0] = (unsigned char) (in[0] << 2 | in[1] >> 4);
    out[1] = (unsigned char) (in[1] << 4 | in[2] >> 2);
    out[2] = (unsigned char) (((in[2] << 6) & 0xc0) | in[3]);
}

/*
** decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
*/
static int decode(FILE *infile, FILE *outfile) {
    int retcode = 0;
    unsigned char in[4];
    unsigned char out[3];
    int v;
    int i, len;

    *in = (unsigned char) 0;
    *out = (unsigned char) 0;
    while (feof(infile) == 0) {
        for (len = 0, i = 0; i < 4 && feof(infile) == 0; i++) {
            v = 0;
            while (feof(infile) == 0 && v == 0) {
                v = getc(infile);
                if (feof(infile) == 0) {
                    v = ((v < 43 || v > 122) ? 0 : (int) cd64[v - 43]);
                    if (v != 0) {
                        v = ((v == (int) '$') ? 0 : v - 61);
                    }
                }
            }
            if (feof(infile) == 0) {
                len++;
                if (v != 0) {
                    in[i] = (unsigned char) (v - 1);
                }
            } else {
                in[i] = (unsigned char) 0;
            }
        }
        if (len > 0) {
            decodeblock(in, out);
            for (i = 0; i < len - 1; i++) {
                if (putc((int) out[i], outfile) == EOF) {
                    if (ferror(outfile) != 0) {
                        retcode = B64_FILE_IO_ERROR;
                    }
                    break;
                }
            }
        }
    }
    return (retcode);
}

static int decode(const char *infilename, const char *outfilename) {
    FILE *infile = fopen(infilename, "rb");
    FILE *outfile = fopen(outfilename, "wb");

    int ret = decode(infile, outfile);
    fclose(infile);
    fclose(outfile);

    return ret;
}

} // namespace common
