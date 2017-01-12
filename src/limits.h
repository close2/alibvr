#pragma once

/**
 * @file
 * 
 * @brief Implementation of std::numeric_limits.
 * 
 * The behaviour is (should be) identical to the standard C++ library.
 **/

namespace std {
  template <typename T>
  class numeric_limits {
  public:
    static uint8_t is_signed();
    static uint8_t digits();
    static T max();
  };
  
  template <>
  class numeric_limits<uint8_t> {
  public:
    static uint8_t is_signed() { return false; }
    static uint8_t digits() { return 8; }
    static uint8_t max() { return 0xFF; }
  };
  
  template <>
  class numeric_limits<uint16_t> {
  public:
    static uint8_t is_signed() { return false; }
    static uint8_t digits() { return 16; }
    static uint16_t max() { return 0xFFFF; }
  };
  
  template <>
  class numeric_limits<uint32_t> {
  public:
    static uint8_t is_signed() { return false; }
    static uint8_t digits() { return 32; }
    static uint32_t max() { return 0xFFFFFFFF; }
  };
  
  template <>
  class numeric_limits<uint64_t> {
  public:
    static uint8_t is_signed() { return false; }
    static uint8_t digits() { return 64; }
    static uint64_t max() { return 0xFFFFFFFFFFFFFFFF; }
  };
}
