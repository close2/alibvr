#pragma once

/**
 * @brief This file contains `typedef`s which make it easier to access
 * specific bytes of multi-byte values.
 * 
 * An `uint16_t` for instance may be cast to `union bits16_s` which
 * makes it possible to access to less significant byte with:
 * `v.avr.lo`.
 * 
 * All `union` `typedef`s provide an `avr` member, with `lo` and `hi`
 * members.  `avr` / gcc is little-endian!
 **/

// -----------------------------------------------------------------------
// Types for easier byte-word-dword conversions
// -----------------------------------------------------------------------

// avr / gcc is little-endian

typedef struct bits16_hilo_s {
  uint8_t hi;
  uint8_t lo;
} bits16_hilo_t;

typedef struct bits16_lohi_s {
  uint8_t lo;
  uint8_t hi;
} bits16_lohi_t;

typedef union bits16_s {
  uint8_t       byte[2];
  uint8_t       uint8[2];
  int8_t        int8[2];
  bits16_lohi_t avr;
  bits16_hilo_t hilo;
  bits16_lohi_t lohi;
  uint16_t      word;
  uint16_t      uint16;
  int16_t       int16;
} bits16_t;

typedef struct bits32_hilo_s {
  bits16_t hi;
  bits16_t lo;
} bits32_hilo_t;

typedef struct bits32_lohi_s {
  bits16_t lo;
  bits16_t hi;
} bits32_lohi_t;

typedef union bits32_t {
  uint8_t       byte[4];
  uint8_t       uint8[4];
  int8_t        int8[4];
  uint16_t      word[2];
  uint16_t      uint16[2];
  int16_t       int16[2];
  bits32_lohi_t avr;
  bits32_hilo_t hilo;
  bits32_lohi_t lohi;
  uint32_t      dword;
  uint32_t      uint32;
  int32_t       int32;
} bits32_t;

typedef struct bits64_hilo_s {
  bits32_t hi;
  bits32_t lo;
} bits64_hilo_t;

typedef struct bits64_lohi_s {
  bits32_t lo;
  bits32_t hi;
} bits64_lohi_t;

typedef union bits64_s {
  uint8_t       byte[8];
  uint8_t       uint8[8];
  int8_t        int8[8];
  uint16_t      word[4];
  uint16_t      uint16[4];
  int16_t       int16[4];
  uint32_t      dword[2];
  uint32_t      uint32[2];
  int32_t       int32[2];
  bits64_lohi_t avr;
  bits64_hilo_t hilo;
  bits64_lohi_t lohi;
  uint64_t      uint64;
  int64_t       int64;
} bits64_t;