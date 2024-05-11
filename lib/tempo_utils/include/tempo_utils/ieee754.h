#ifndef TEMPO_UTILS_IEEE_754_H
#define TEMPO_UTILS_IEEE_754_H

#include <vector>

#include "big_endian.h"

namespace tempo_utils {

    double read_dbl(const tu_uint8 array[8]);
    void write_dbl(double dbl, tu_uint8 array[8]);

    /**
     * Read a double from the given ptr and advance ptr by 8 bytes.
     *
     * @param ptr
     * @return
     */
    inline double read_dbl_and_advance(const tu_uint8 *&ptr) {
        double result = read_dbl(ptr);
        ptr += 8;
        return result;
    }
}

#endif // TEMPO_UTILS_IEEE_754_H
