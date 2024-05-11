
#include <cstring>

#include <tempo_utils/big_endian.h>

/**
 * Read a big-endian encoded uint16 from `array` and return the value after converting to host order.
 * Note that this function does not perform any bounds checking on `array`.
 *
 * @param array pointer to an array containing at least 2 bytes.
 * @return the unsigned 16bit value.
 */
tu_uint16
tempo_utils::read_u16(const tu_uint8 array[2])
{
    tu_uint16 ns = 0;
    auto *ptr = (tu_uint8 *) &ns;
    memcpy(ptr, array, 2);
    return BE16_TO_H(ns);
}

/**
 * Read a big-endian encoded int16 from `array` and return the value after converting to host order.
 * Note that this function does not perform any bounds checking on `array`.
 *
 * @param array pointer to an array containing at least 2 bytes.
 * @return the signed 16bit value.
 */
tu_int16
tempo_utils::read_i16(const tu_uint8 array[2])
{
    tu_int16 ns = 0;
    auto *ptr = (tu_uint8 *) &ns;
    memcpy(ptr, array, 2);
    return BE16_TO_H(ns);
}

/**
 * Read a big-endian encoded uint32 from `array` and return the value after converting to host order.
 * Note that this function does not perform any bounds checking on `array`.
 *
 * @param array pointer to an array containing at least 4 bytes.
 * @return the unsigned 32bit value.
 */
tu_uint32
tempo_utils::read_u32(const tu_uint8 array[4])
{
    tu_uint32 nl = 0;
    auto *ptr = (tu_uint8 *) &nl;
    memcpy(ptr, array, 4);
    return BE32_TO_H(nl);
}

/**
 * Read a big-endian encoded int32 from `array` and return the value after converting to host order.
 * Note that this function does not perform any bounds checking on `array`.
 *
 * @param array pointer to an array containing at least 4 bytes.
 * @return the signed 32bit value.
 */
tu_int32
tempo_utils::read_i32(const tu_uint8 array[4])
{
    tu_int32 nl = 0;
    auto *ptr = (tu_uint8 *) &nl;
    memcpy(ptr, array, 4);
    return BE32_TO_H(nl);
}

/**
 * Read a big-endian encoded uint64 from `array` and return the value after converting to host order.
 * Note that this function does not perform any bounds checking on `array`.
 *
 * @param array pointer to an array containing at least 8 bytes.
 * @return the unsigned 64bit value.
 */
tu_uint64
tempo_utils::read_u64(const tu_uint8 array[8])
{
    tu_uint64 nll = 0;
    auto *ptr = (tu_uint8 *) &nll;
    memcpy(ptr, array, 8);
    return BE64_TO_H(nll);
}

/**
 * Read a big-endian encoded int64 from `array` and return the value after converting to host order.
 * Note that this function does not perform any bounds checking on `array`.
 *
 * @param array pointer to an array containing at least 8 bytes.
 * @return the signed 64bit value.
 */
tu_int64
tempo_utils::read_i64(const tu_uint8 array[8])
{
    tu_int64 nll = 0;
    auto *ptr = (tu_uint8 *) &nll;
    memcpy(ptr, array, 8);
    return BE64_TO_H(nll);
}

void
tempo_utils::write_i16(tu_int16 i16, tu_uint8 array[2])
{
    i16 = H_TO_BE16(i16);
    auto *ptr = (const tu_uint8 *) &i16;
    memcpy(array, ptr, 2);
}

void
tempo_utils::write_u16(tu_uint16 u16, tu_uint8 array[2])
{
    u16 = H_TO_BE16(u16);
    auto *ptr = (const tu_uint8 *) &u16;
    memcpy(array, ptr, 2);
}

void
tempo_utils::write_i32(tu_int32 i32, tu_uint8 array[4])
{
    i32 = H_TO_BE32(i32);
    auto *ptr = (const tu_uint8 *) &i32;
    memcpy(array, ptr, 4);
}

void
tempo_utils::write_u32(tu_uint32 u32, tu_uint8 array[4])
{
    u32 = H_TO_BE32(u32);
    auto *ptr = (const tu_uint8 *) &u32;
    memcpy(array, ptr, 4);
}

void
tempo_utils::write_i64(tu_int64 i64, tu_uint8 array[8])
{
    i64 = H_TO_BE64(i64);
    auto *ptr = (const tu_uint8 *) &i64;
    memcpy(array, ptr, 8);
}

void
tempo_utils::write_u64(tu_uint64 u64, tu_uint8 array[8])
{
    u64 = H_TO_BE64(u64);
    auto *ptr = (const tu_uint8 *) &u64;
    memcpy(array, ptr, 8);
}
