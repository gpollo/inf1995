#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette macro ajuste un word big-endian en un word little-endian.
 *
 * @param n L'octet à convertir.
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define FLIP_WORD(n) ((((n) & 0x00FF) << 8) | (((n) & 0xFF00) >> 8));
#endif

#ifdef __cplusplus
}
#endif

#endif
