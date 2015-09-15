#pragma once

#include "macros.h"

#define C_SET8(reg, cache, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0) \
  do { \
    uint8_t _tmpCacheB = cache##B; \
    uint8_t _tmpCacheC = cache##C; \
    uint8_t _tmpCacheD = cache##D; \
    SET8_TMP(cache, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0); \
    if (cache##B != _tmpCacheB) { \
      reg##B = cache##B; \
    } \
    if (cache##C != _tmpCacheC) { \
      reg##C = cache##C; \
    } \
    if (cache##D != _tmpCacheD) { \
      reg##D = cache##D; \
    } \
  } while(0)

#define C_SET8_BYTE(reg, cache, val, dest_pin7, dest_pin6, dest_pin5, dest_pin4, dest_pin3, dest_pin2, dest_pin1, dest_pin0) C_SET8(reg, cache, val, dest_pin7, 7, dest_pin6, 6, dest_pin5, 5, dest_pin4, 4, dest_pin3, 3, dest_pin2, 2, dest_pin1, 1, dest_pin0, 0)
#define C_SET4_NIBBLE(reg, cache, val, offset, dest_pin3, dest_pin2, dest_pin1, dest_pin0) C_SET8(reg, cache, val, dest_pin0, -1, dest_pin0, -1, dest_pin0, -1, dest_pin0, -1, dest_pin3, 3 + offset, dest_pin2, 2 + offset, dest_pin1, 1 + offset, dest_pin0, 0 + offset)

#define LCACHE_PORT _lPortCache
#define DEF_LCACHE_PORT \
  uint8_t _lPortCacheB = PORTB; \
  uint8_t _lPortCacheC = PORTC; \
  uint8_t _lPortCacheD = PORTD;
  