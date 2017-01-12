#pragma once
#include <stdint.h>

namespace type_comparisons {
  uint8_t bigger_type(uint8_t, uint8_t);
  uint16_t bigger_type(uint8_t, uint16_t);
  uint16_t bigger_type(uint16_t, uint8_t);
  uint16_t bigger_type(uint16_t, uint16_t);
  uint32_t bigger_type(uint8_t, uint32_t);
  uint32_t bigger_type(uint32_t, uint8_t);
  uint32_t bigger_type(uint16_t, uint32_t);
  uint32_t bigger_type(uint32_t, uint16_t);
  uint32_t bigger_type(uint32_t, uint32_t);
  
  uint64_t bigger_type(uint64_t, uint64_t);
  template <typename T>
  uint64_t bigger_type(uint64_t, T);
  template <typename T>
  uint64_t bigger_type(T, uint64_t);
}
