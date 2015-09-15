#pragma once

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
}