#pragma once

#include <avr/pgmspace.h>

/* BALI notes
 * add automatic conversion to const
 */

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
