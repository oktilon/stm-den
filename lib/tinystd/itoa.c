#include "itoa.h"

unsigned long itoas(long value, char *sp, int radix) {
    unsigned long len;
    char          tmp[32], *tp = tmp;
    long          i, v, sign   = radix == 10 && value < 0;

    v = sign ? -value : value;

    while (v || tp == tmp) {
        i = v % radix;
        v /= radix;
        *tp++ = i < 10 ? (char) (i + '0') : (char) (i + 'a' - 10);
    }


    len = tp - tmp;

    if (sign) {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    *sp++ = '\0';

    return len;
}
