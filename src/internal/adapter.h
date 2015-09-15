#pragma once

class Adapter {
  
public:
  static uint8_t v4bit_to_hex_char(const uint8_t& n) {
    // n is a 4bit value
    if (n < 10) {
      return '0' + n;
    } else {
      return 'A' + n - 10;
    }
  }

  // -1 in case of error
  static uint8_t hex_to_v4bit(const uint8_t& c) {
    if (c >= '0' && c <= '9') {
      return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
      return c - 'A' + 10;
    }
    return -1;
  }

private:
  template <uint32_t k, typename N>
  static inline uint8_t sub_and_count(N& n) {
    uint8_t i = 0;
    while (n >= k) {
      n -= k;
      i++;
    }
    return i;
  }

public:
  template <void (&F)(uint8_t)>
  static void apply(const PgmDataPtr<char>* const& pmemS) {
    const uint8_t* s = (const uint8_t*) pmemS;
    while (const uint8_t c = pgm_read_byte(s++)) {
      F(c);
    }
  }

  template <void (&F)(uint8_t), uint8_t size, typename T>
  static void apply(const PgmDataPtr<T>* const& pmemS) {
    T t;
    uint8_t* runner = (uint8_t*) &t;
    const uint8_t* m = (const uint8_t*) pmemS;
    
    uint8_t i = 0;
    while (i < size) {
      runner++ = pgm_read_byte(m++);
      i++;
    }
    
    apply<F>(t);
  }
  
  template <void (&F)(uint8_t)>
  static void apply(const char* const& str) {
    const uint8_t* s = (const uint8_t*) str;
    while (const uint8_t c = *s++) {
      F(c);
    }
  }
  
  template <void (&F)(uint8_t)>
  static void apply(const char& c) {
    F((uint8_t) c);
  }
  
  template <void (&F)(uint8_t)>
  static void apply(const PgmDataPtr<uint8_t>* const pmemS) {
    apply<F, 1>(pmemS);
  }
  
  template <void (&F)(uint8_t)>
  static void apply(const PgmDataPtr<uint16_t>* const pmemS) {
    apply<F, 2>(pmemS);
  }
  
  template <void (&F)(uint8_t)>
  static void apply(const PgmDataPtr<uint32_t>* const pmemS) {
    apply<F, 4>(pmemS);
  }
  
  template <void (&F)(uint8_t)>
  static void apply(const PgmDataPtr<int8_t>* const pmemS) {
    apply<F, 1>(pmemS);
  }
  
  template <void (&F)(uint8_t)>
  static void apply(const PgmDataPtr<int16_t>* const pmemS) {
    apply<F, 2>(pmemS);
  }

  template <void (&F)(uint8_t)>
  static void apply(const PgmDataPtr<int32_t>* const pmemS) {
    apply<F, 4>(pmemS);
  }

  template <void (&F)(uint8_t)>
  static inline void apply(const uint8_t& n8, uint8_t hex = 1) {
    uint8_t n = n8;
    
    if (hex) {
      F(v4bit_to_hex_char(n >> 4));
      F(v4bit_to_hex_char(n & 0x0F));
    } else {
      if (n >= 200) {
        F('2');
        n -= 200;
      } else if (n >= 100) {
        F('1');
        n -= 100;
      }

      uint8_t i = 0;
      while (n >= 10) {
        i++;
        n -= 10;
      }
      if (i > 0) {
        F('0' + i);
      }

      F(n);
    }
  }
  
  template <void (&F)(uint8_t)>
  static inline void apply(const uint16_t& n16, uint8_t hex = 1) {
    uint16_t n = n16;

    if (hex) {
      uint8_t hi = n >> 8;
      uint8_t lo = n & 0xFF;
      F(v4bit_to_hex_char(hi >> 4));
      F(v4bit_to_hex_char(hi & 0x0F));
      F(v4bit_to_hex_char(lo >> 4));
      F(v4bit_to_hex_char(lo & 0x0F));
    } else {
      uint8_t starts_with_0 = 1;
      
      uint8_t i = sub_and_count<10000>(n);
      if (i > 0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<1000>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<100>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<10>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      F('0' + n);
    }
  }

  template <void (&F)(uint8_t)>
  static inline void apply(const uint32_t& n32, uint8_t hex = 0) {
    uint32_t n = n32;

    if (hex) {
      uint8_t b3 = (n >> 24) & 0xFF;
      uint8_t b2 = (n >> 16) & 0xFF;
      uint8_t b1 = (n >> 8) & 0xFF;
      uint8_t b0 = n & 0xFF;
      F(v4bit_to_hex_char(b3 >> 4));
      F(v4bit_to_hex_char(b3 & 0x0F));
      F(v4bit_to_hex_char(b2 >> 4));
      F(v4bit_to_hex_char(b2 & 0x0F));
      F(v4bit_to_hex_char(b1 >> 4));
      F(v4bit_to_hex_char(b1 & 0x0F));
      F(v4bit_to_hex_char(b0 >> 4));
      F(v4bit_to_hex_char(b0 & 0x0F));
    } else {
      uint8_t starts_with_0 = 1;

      uint8_t i = sub_and_count<1000000000>(n);
      if (i > 0) {
        starts_with_0 = 0;
        F('0' + i);
      }

      i = sub_and_count<100000000>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<10000000>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<1000000>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<100000>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<10000>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<1000>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<100>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      i = sub_and_count<10>(n);
      if (i > 0 || !starts_with_0) {
        starts_with_0 = 0;
        F('0' + i);
      }
      
      F('0' + n);
    }
  }
};