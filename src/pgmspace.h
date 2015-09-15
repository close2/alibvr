#pragma once

#include <avr/pgmspace.h>

template <typename T = uint8_t>
class PgmDataPtr {
public:
  typedef const T* type_t;

  type_t get() const {
    return (type_t) this;
  }
  
  template <typename V>
  static const PgmDataPtr<V>* ptr(const V* v) {
    return (const PgmDataPtr<V>*) v;
  }
};
