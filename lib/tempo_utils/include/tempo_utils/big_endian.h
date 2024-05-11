#ifndef TEMPO_UTILS_BIG_ENDIAN_H
#define TEMPO_UTILS_BIG_ENDIAN_H

#ifdef __APPLE__

#include <machine/endian.h>
#define H_TO_BE16(x)    htons(x)
#define H_TO_BE32(x)    htonl(x)
#define H_TO_BE64(x)    htonll(x)
#define BE16_TO_H(x)    ntohs(x)
#define BE32_TO_H(x)    ntohl(x)
#define BE64_TO_H(x)    ntohll(x)

#elif __linux__

#include <arpa/inet.h>
#include <endian.h>

#define H_TO_BE16(x)    htobe16(x)
#define H_TO_BE32(x)    htobe32(x)
#define H_TO_BE64(x)    htobe64(x)
#define BE16_TO_H(x)    be16toh(x)
#define BE32_TO_H(x)    be32toh(x)
#define BE64_TO_H(x)    be64toh(x)

#endif

#include <vector>

#include "integer_types.h"

namespace tempo_utils {

    tu_uint16 read_u16(const tu_uint8 array[2]);
    tu_int16 read_i16(const tu_uint8 array[2]);
    tu_uint32 read_u32(const tu_uint8 array[4]);
    tu_int32 read_i32(const tu_uint8 array[4]);
    tu_uint64 read_u64(const tu_uint8 array[8]);
    tu_int64 read_i64(const tu_uint8 array[8]);

    void write_i16(tu_int16 u16, tu_uint8 array[2]);
    void write_u16(tu_uint16 u16, tu_uint8 array[2]);
    void write_i32(tu_int32 u32, tu_uint8 array[4]);
    void write_u32(tu_uint32 u32, tu_uint8 array[4]);
    void write_i64(tu_int64 u64, tu_uint8 array[8]);
    void write_u64(tu_uint64 u64, tu_uint8 array[8]);

    /**
     * Read a uint8 from the given ptr and advance ptr by 1 byte.
     *
     * @param ptr
     * @return
     */
    inline tu_uint8 read_u8_and_advance(const tu_uint8 *&ptr) {
        tu_uint8 result = *ptr;
        ptr += 1;
        return result;
    }

    /**
     * Read an int8 from the given ptr and advance ptr by 1 byte.
     *
     * @param ptr
     * @return
     */
    inline tu_int8 read_i8_and_advance(const tu_uint8 *&ptr) {
        tu_int8 result = (tu_int8) *ptr;
        ptr += 1;
        return result;
    }

    /**
     * Read a uint16 from the given ptr and advance ptr by 2 bytes.
     *
     * @param ptr
     * @return
     */
    inline tu_uint16 read_u16_and_advance(const tu_uint8 *&ptr) {
        tu_uint16 result = read_u16(ptr);
        ptr += 2;
        return result;
    }

    /**
     * Read an int16 from the given ptr and advance ptr by 2 bytes.
     *
     * @param ptr
     * @return
     */
    inline tu_int16 read_i16_and_advance(const tu_uint8 *&ptr) {
        tu_int16 result = read_i16(ptr);
        ptr += 2;
        return result;
    }

    /**
     * Read a uint32 from the given ptr and advance ptr by 4 bytes.
     *
     * @param ptr
     * @return
     */
    inline tu_uint32 read_u32_and_advance(const tu_uint8 *&ptr) {
        tu_uint32 result = read_u32(ptr);
        ptr += 4;
        return result;
    }

    /**
     * Read an int32 from the given ptr and advance ptr by 4 bytes.
     *
     * @param ptr
     * @return
     */
    inline tu_int32 read_i32_and_advance(const tu_uint8 *&ptr) {
        tu_int32 result = read_i32(ptr);
        ptr += 4;
        return result;
    }

    /**
     * Read a uint64 from the given ptr and advance ptr by 8 bytes.
     *
     * @param ptr
     * @return
     */
    inline tu_uint64 read_u64_and_advance(const tu_uint8 *&ptr) {
        tu_uint64 result = read_u64(ptr);
        ptr += 8;
        return result;
    }

    /**
     * Read an int64 from the given ptr and advance ptr by 8 bytes.
     *
     * @param ptr
     * @return
     */
    inline tu_int64 read_i64_and_advance(const tu_uint8 *&ptr) {
        tu_int64 result = read_i64(ptr);
        ptr += 8;
        return result;
    }
}

#endif // TEMPO_UTILS_BIG_ENDIAN_H
