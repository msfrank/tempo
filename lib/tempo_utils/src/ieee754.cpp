/**
 * shamelessly adapted from Beej's Guide to Network Programming
 *
 *   https://beej.us/guide/bgnet/
 *   https://beej.us/guide/bgnet/examples/ieee754.c
 *
 */

#include <tempo_utils/ieee754.h>

#define EXP_BITS    ((unsigned) 11)
#define BITS        ((unsigned) 64)

double
tempo_utils::read_dbl(const tu_uint8 array[8])
{
    double result;
    tu_int64 shift;
    unsigned bias;
    unsigned significandbits = BITS - EXP_BITS - 1;     // -1 for sign bit

    auto i = read_u64(array);
    if (i == 0) return 0.0;

    // pull the significand
    result = (i & ((1LL << significandbits) - 1));      // mask
    result /= (1LL << significandbits);                 // convert back to float
    result += 1.0f;                                     // add the one back on

    // deal with the exponent
    bias = (1 << (EXP_BITS - 1)) - 1;
    shift = ((i >> significandbits) & ((1LL << EXP_BITS) - 1)) - bias;
    while (shift > 0) { result *= 2.0; shift--; }
    while (shift < 0) { result /= 2.0; shift++; }

    // sign it
    result *= (i >> (BITS - 1)) & 1 ? -1.0 : 1.0;

    return result;
}

void
tempo_utils::write_dbl(double dbl, tu_uint8 array[8])
{
    long double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = BITS - EXP_BITS - 1; // -1 for sign bit

    tu_uint64 u64;

    if (dbl == 0.0) {       // get this special case out of the way
        u64 = 0;
    } else {
        // check sign and begin normalization
        if (dbl < 0) { sign = 1; fnorm = -dbl; }
        else { sign = 0; fnorm = dbl; }

        // get the normalized form of f and track the exponent
        shift = 0;
        while (fnorm >= 2.0) { fnorm /= 2.0; shift++; }
        while (fnorm < 1.0) { fnorm *= 2.0; shift--; }
        fnorm = fnorm - 1.0;

        // calculate the binary form (non-float) of the significand data
        significand = fnorm * ((1LL<<significandbits) + 0.5f);

        // get the biased exponent
        exp = shift + ((1 << (EXP_BITS - 1)) - 1); // shift + bias

        // return the final answer
        u64 = (sign << (BITS - 1)) | (exp << (BITS - EXP_BITS - 1)) | significand;
    }

    write_u64(u64, array);
}