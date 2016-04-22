#pragma once

#include <avr/sfr_defs.h>
#include <avr/io.h>


// PIN layout is compatible to arduino / arduino lite
//                                      +-\/-+
//             PCINT14            PC6  1|    |28  PC5 (AI 5/*D19) PCINT13 ADC5 SCL
// RXD         PCINT16      (D 0) PD0  2|    |27  PC4 (AI 4/*D18) PCINT12 ADC4 SDA
// TXD         PCINT17      (D 1) PD1  3|    |26  PC3 (AI 3/*D17) PCINT11 ADC3
//       INT0  PCINT18      (D 2) PD2  4|    |25  PC2 (AI 2/*D16) PCINT10 ADC2
// OC2B  INT1  PCINT19 PWM+ (D 3) PD3  5|    |24  PC1 (AI 1/*D15) PCINT9  ADC1
// XCK   T0    PCINT20      (D 4) PD4  6|    |23  PC0 (AI 0/*D14) PCINT8  ADC0
//                                VCC  7|    |22  GND
//                                GND  8|    |21  AREF
// XTAL1 TOSC1 PCINT6      *(D20) PB6  9|    |20  AVCC
// XTAL2 TOSC2 PCINT7      *(D21) PB7 10|    |19  PB5 (D 13)      PCINT5  SCK
// OC0B  T1    PCINT21 PWM+ (D 5) PD5 11|    |18  PB4 (D 12)      PCINT4  MISO
// OC0A  AIN0  PCINT22 PWM+ (D 6) PD6 12|    |17  PB3 (D 11) PWM  PCINT3  MOSI OC2A
//       AIN1  PCINT23      (D 7) PD7 13|    |16  PB2 (D 10) PWM  PCINT2  SS   OC1B
// CLKO  ICP1  PCINT0       (D 8) PB0 14|    |15  PB1 (D 9)  PWM  PCINT1       OC1A
//                                      +----+


namespace _ports {
  enum class Port {
    B,
    C,
    D
  };
  
  enum class IOReg {
    DDRx,
    PORTx,
    PINx
  };
  
  template <enum Port p, uint8_t b, enum IOReg io>
  struct _Io;
  
  template <enum Port p, uint8_t b>
  struct Pin {
    static const enum Port port = p;
    static const uint8_t   bit = b;
    
    typedef _Io<p, b, IOReg::DDRx>  _DDR;
    static _DDR DDR;
    
    typedef _Io<p, b, IOReg::PORTx> _PORT;
    static _PORT PORT;
    
    typedef _Io<p, b, IOReg::PINx>  _PIN;
    static _PIN PIN;
  };
}

typedef struct _ports::Pin<_ports::Port::B, -1> PIN_UNUSED;

typedef struct _ports::Pin<_ports::Port::D, 0 > PIN_0 ;
typedef struct _ports::Pin<_ports::Port::D, 1 > PIN_1 ;
typedef struct _ports::Pin<_ports::Port::D, 2 > PIN_2 ;
typedef struct _ports::Pin<_ports::Port::D, 3 > PIN_3 ;
typedef struct _ports::Pin<_ports::Port::D, 4 > PIN_4 ;
typedef struct _ports::Pin<_ports::Port::D, 5 > PIN_5 ;
typedef struct _ports::Pin<_ports::Port::D, 6 > PIN_6 ;
typedef struct _ports::Pin<_ports::Port::D, 7 > PIN_7 ;
typedef struct _ports::Pin<_ports::Port::B, 0 > PIN_8 ;
typedef struct _ports::Pin<_ports::Port::B, 1 > PIN_9 ;
typedef struct _ports::Pin<_ports::Port::B, 2 > PIN_10;
typedef struct _ports::Pin<_ports::Port::B, 3 > PIN_11;
typedef struct _ports::Pin<_ports::Port::B, 4 > PIN_12;
typedef struct _ports::Pin<_ports::Port::B, 5 > PIN_13;
typedef struct _ports::Pin<_ports::Port::C, 0 > PIN_14;
typedef struct _ports::Pin<_ports::Port::C, 1 > PIN_15;
typedef struct _ports::Pin<_ports::Port::C, 2 > PIN_16;
typedef struct _ports::Pin<_ports::Port::C, 3 > PIN_17;
typedef struct _ports::Pin<_ports::Port::C, 4 > PIN_18;
typedef struct _ports::Pin<_ports::Port::C, 5 > PIN_19;
typedef struct _ports::Pin<_ports::Port::B, 6 > PIN_20;
typedef struct _ports::Pin<_ports::Port::B, 7 > PIN_21;

typedef struct _ports::Pin<_ports::Port::C, 6 > PIN_DIP_1;
typedef PIN_0  PIN_DIP_2 ;
typedef PIN_1  PIN_DIP_3 ;
typedef PIN_2  PIN_DIP_4 ;
typedef PIN_3  PIN_DIP_5 ;
typedef PIN_4  PIN_DIP_6 ;
typedef PIN_20 PIN_DIP_9 ;
typedef PIN_21 PIN_DIP_10;
typedef PIN_5  PIN_DIP_11;
typedef PIN_6  PIN_DIP_12;
typedef PIN_7  PIN_DIP_13;
typedef PIN_8  PIN_DIP_14;
typedef PIN_9  PIN_DIP_15;
typedef PIN_10 PIN_DIP_16;
typedef PIN_11 PIN_DIP_17;
typedef PIN_12 PIN_DIP_18;
typedef PIN_13 PIN_DIP_19;
typedef PIN_14 PIN_DIP_23;
typedef PIN_15 PIN_DIP_24;
typedef PIN_16 PIN_DIP_25;
typedef PIN_17 PIN_DIP_26;
typedef PIN_18 PIN_DIP_27;
typedef PIN_19 PIN_DIP_28;

// ===========================
// USART
typedef PIN_DIP_2  PIN_RXD;
typedef PIN_DIP_3  PIN_TXD;
typedef PIN_DIP_6  PIN_XCK; // USART external clock

// Oscillators
// Chip clock Oscillator
typedef PIN_DIP_9  PIN_XTAL1;
typedef PIN_DIP_10 PIN_XTAL2;
// Timer Oscillator
typedef PIN_DIP_9  PIN_TOSC1;
typedef PIN_DIP_10 PIN_TOSC2;

// System clock
typedef PIN_DIP_14 PIN_CLK0;

// SPI
typedef PIN_DIP_17 PIN_MOSI;
typedef PIN_DIP_18 PIN_MISO;
typedef PIN_DIP_16 PIN_SS; // SS' !
typedef PIN_DIP_19 PIN_SCK;

// TWI
typedef PIN_DIP_28 PIN_SCL; // 2 wire Serial Interface Clock
typedef PIN_DIP_27 PIN_SDA; // 2 wire Serial Data

// Timer/Counter
// Timer0
typedef PIN_DIP_6  PIN_T0;   // Counter source
typedef PIN_DIP_11 PIN_OC0B; // Output compare match
typedef PIN_DIP_12 PIN_OC0A;

// Timer1
typedef PIN_DIP_11 PIN_T1;   // Counter source
typedef PIN_DIP_15 PIN_OC1A; // Output compare match
typedef PIN_DIP_16 PIN_OC1B;
typedef PIN_DIP_14 PIN_ICP1; // Input capture pin

// Timer2
typedef PIN_DIP_17 PIN_OC2A; // Output compare match
typedef PIN_DIP_5  PIN_OC2B;

// ADC
typedef PIN_DIP_23 PIN_ADC0;
typedef PIN_DIP_24 PIN_ADC1;
typedef PIN_DIP_25 PIN_ADC2;
typedef PIN_DIP_26 PIN_ADC3;
typedef PIN_DIP_27 PIN_ADC4;
typedef PIN_DIP_28 PIN_ADC5;

typedef PIN_DIP_12 PIN_AIN0; // Analog comparator negative input
typedef PIN_DIP_13 PIN_AIN1; // Analog comparator positive input

// External interrupt sources
typedef PIN_DIP_4  PIN_INT0;
typedef PIN_DIP_5  PIN_INT1;
// ===========================

typedef PIN_DIP_14 PIN_B0;
typedef PIN_DIP_15 PIN_B1;
typedef PIN_DIP_16 PIN_B2;
typedef PIN_DIP_17 PIN_B3;
typedef PIN_DIP_18 PIN_B4;
typedef PIN_DIP_19 PIN_B5;
typedef PIN_DIP_9  PIN_B6;
typedef PIN_DIP_10 PIN_B7;

typedef PIN_DIP_23 PIN_C0;
typedef PIN_DIP_24 PIN_C1;
typedef PIN_DIP_25 PIN_C2;
typedef PIN_DIP_26 PIN_C3;
typedef PIN_DIP_27 PIN_C4;
typedef PIN_DIP_28 PIN_C5;
typedef PIN_DIP_1  PIN_C6;

typedef PIN_DIP_2  PIN_D0;
typedef PIN_DIP_3  PIN_D1;
typedef PIN_DIP_4  PIN_D2;
typedef PIN_DIP_5  PIN_D3;
typedef PIN_DIP_6  PIN_D4;
typedef PIN_DIP_11 PIN_D5;
typedef PIN_DIP_12 PIN_D6;
typedef PIN_DIP_13 PIN_D7;


namespace _ports {
  enum class _Read_Write {
    Read,
    Write
  };
  
  template <class Io, enum _Read_Write RW>
  uint8_t _set_or_get(uint8_t val);
  
  template <enum Port p, uint8_t b, enum IOReg io>
  struct _Io {
    typedef _Io<p, b, io> _Io_t;
    
    static const enum Port port = p;
    static const uint8_t bit = b;
    static const enum IOReg io_reg = io;
    
    _Io_t& operator=(const uint8_t val) {
      _set_or_get<_Io_t, _Read_Write::Write>(val);
      return *this;
    }
    
    operator uint8_t() {
      return _set_or_get<_Io_t, _Read_Write::Read>(0);
    }
  };
  
  template <enum _Read_Write RW, class R>
  uint8_t _set_or_get_f(R& reg, uint8_t bit, uint8_t value) {
    if (RW == _Read_Write::Write) {
      if (value) reg |= _BV(bit);
      else reg &= ~_BV(bit);
      return 0;
    } else return reg & _BV(bit);
  }
  
  template <class Io, enum _Read_Write RW>
  uint8_t _set_or_get(uint8_t val) {
    static_assert(Io::io_reg == IOReg::DDRx ||
                  Io::io_reg == IOReg::PINx ||
                  Io::io_reg == IOReg::PORTx,
                  "IO Register is unknown.  Only Ddr, Pin and Port are supported.");
    switch(Io::io_reg) {
      case IOReg::DDRx:
        switch(Io::port) {
          case Port::B:
            return _set_or_get_f<RW>(DDRB, Io::bit, val);
          case Port::C:
            return _set_or_get_f<RW>(DDRC, Io::bit, val);
          case Port::D:
            return _set_or_get_f<RW>(DDRD, Io::bit, val);
        }
        break;
      case IOReg::PORTx:
        switch(Io::port) {
          case Port::B:
            return _set_or_get_f<RW>(PORTB, Io::bit, val);
          case Port::C:
            return _set_or_get_f<RW>(PORTC, Io::bit, val);
          case Port::D:
            return _set_or_get_f<RW>(PORTD, Io::bit, val);
        }
        break;
      case IOReg::PINx:
        switch(Io::port) {
          case Port::B:
            return _set_or_get_f<RW>(PINB, Io::bit, val);
          case Port::C:
            return _set_or_get_f<RW>(PINC, Io::bit, val);
          case Port::D:
            return _set_or_get_f<RW>(PIND, Io::bit, val);
        }
        break;
    }
    return 0;  // should never happen
  }

}


namespace _ports {
  // V .. value
  // P .. port variables (in write mode Dest, in read mode Src)
  // Bn .. bit in value (in write mode Src, in read mode Dest)
  // Dn .. port and bit in Px (depending on enum) (in write mode Dest, in read mode Src)
  template <class D7, uint8_t B7,
            class D6, uint8_t B6,
            class D5, uint8_t B5,
            class D4, uint8_t B4,
            class D3, uint8_t B3,
            class D2, uint8_t B2,
            class D1, uint8_t B1,
            class D0, uint8_t B0,
            enum _Read_Write RW, typename V, typename P>
  inline static uint8_t _set_or_get_8_bits(P& portB __attribute__ ((unused)), P& portC __attribute__ ((unused)), P& portD __attribute__ ((unused)), const V& val) {
      uint8_t vB __attribute__ ((unused)) = 0;
      uint8_t vC __attribute__ ((unused)) = 0;
      uint8_t vD __attribute__ ((unused)) = 0;
      uint8_t maskB = 0;
      uint8_t maskC = 0;
      uint8_t maskD = 0;
      
      const uint8_t d0isB = D0::port == _ports::Port::B; const uint8_t d0isC = D0::port == _ports::Port::C;
      const uint8_t d1isB = D1::port == _ports::Port::B; const uint8_t d1isC = D1::port == _ports::Port::C;
      const uint8_t d2isB = D2::port == _ports::Port::B; const uint8_t d2isC = D2::port == _ports::Port::C;
      const uint8_t d3isB = D3::port == _ports::Port::B; const uint8_t d3isC = D3::port == _ports::Port::C;
      const uint8_t d4isB = D4::port == _ports::Port::B; const uint8_t d4isC = D4::port == _ports::Port::C;
      const uint8_t d5isB = D5::port == _ports::Port::B; const uint8_t d5isC = D5::port == _ports::Port::C;
      const uint8_t d6isB = D6::port == _ports::Port::B; const uint8_t d6isC = D6::port == _ports::Port::C;
      const uint8_t d7isB = D7::port == _ports::Port::B; const uint8_t d7isC = D7::port == _ports::Port::C;

      uint8_t& v0 = d0isB ? vB : (d0isC ? vC : vD);
      uint8_t& v1 = d1isB ? vB : (d1isC ? vC : vD);
      uint8_t& v2 = d2isB ? vB : (d2isC ? vC : vD);
      uint8_t& v3 = d3isB ? vB : (d3isC ? vC : vD);
      uint8_t& v4 = d4isB ? vB : (d4isC ? vC : vD);
      uint8_t& v5 = d5isB ? vB : (d5isC ? vC : vD);
      uint8_t& v6 = d6isB ? vB : (d6isC ? vC : vD);
      uint8_t& v7 = d7isB ? vB : (d7isC ? vC : vD);

      uint8_t& mask0 = d0isB ? maskB : (d0isC ? maskC : maskD);
      uint8_t& mask1 = d1isB ? maskB : (d1isC ? maskC : maskD);
      uint8_t& mask2 = d2isB ? maskB : (d2isC ? maskC : maskD);
      uint8_t& mask3 = d3isB ? maskB : (d3isC ? maskC : maskD);
      uint8_t& mask4 = d4isB ? maskB : (d4isC ? maskC : maskD);
      uint8_t& mask5 = d5isB ? maskB : (d5isC ? maskC : maskD);
      uint8_t& mask6 = d6isB ? maskB : (d6isC ? maskC : maskD);
      uint8_t& mask7 = d7isB ? maskB : (d7isC ? maskC : maskD);

      // avoid warning: left shift count >= width of type
      // even if not used.  (if compiled without optimization)
      if (D0::bit < 8 && B0 < 8) {
        const uint8_t shift0 = (D0::bit >= 8) ? 0 : D0::bit;
        mask0 |= 1 << shift0;
        if (RW == _ports::_Read_Write::Write) v0 |= val & _BV(B0);
      }
      if (D1::bit < 8 && B1 < 8) {
        const uint8_t shift1 = (D1::bit >= 8) ? 0 : D1::bit;
        mask1 |= 1 << shift1;
        if (RW == _ports::_Read_Write::Write) v1 |= val & _BV(B1);
      }
      if (D2::bit < 8 && B2 < 8) {
        const uint8_t shift2 = (D2::bit >= 8) ? 0 : D2::bit;
        mask2 |= 1 << shift2;
        if (RW == _ports::_Read_Write::Write) v2 |= val & _BV(B2);
      }
      if (D3::bit < 8 && B3 < 8) {
        const uint8_t shift3 = (D3::bit >= 8) ? 0 : D3::bit;
        mask3 |= 1 << shift3;
        if (RW == _ports::_Read_Write::Write) v3 |= val & _BV(B3);
      }
      if (D4::bit < 8 && B4 < 8) {
        const uint8_t shift4 = (D4::bit >= 8) ? 0 : D4::bit;
        mask4 |= 1 << shift4;
        if (RW == _ports::_Read_Write::Write) v4 |= val & _BV(B4);
      }
      if (D5::bit < 8 && B5 < 8) {
        const uint8_t shift5 = (D5::bit >= 8) ? 0 : D5::bit;
        mask5 |= 1 << shift5;
        if (RW == _ports::_Read_Write::Write) v5 |= val & _BV(B5);
      }
      if (D6::bit < 8 && B6 < 8) {
        const uint8_t shift6 = (D6::bit >= 8) ? 0 : D6::bit;
        mask6 |= 1 << shift6;
        if (RW == _ports::_Read_Write::Write) v6 |= val & _BV(B6);
      }
      if (D7::bit < 8 && B7 < 8) {
        const uint8_t shift7 = (D7::bit >= 8) ? 0 : D7::bit;
        mask7 |= 1 << shift7;
        if (RW == _ports::_Read_Write::Write) v7 |= val & _BV(B7);
      }
  
      if (maskB != 0) {
        if (RW == _ports::_Read_Write::Write) portB = (portB & ~maskB) | vB;
        else vB = portB;
      }
      if (maskC != 0) {
        if (RW == _ports::_Read_Write::Write) portC = (portC & ~maskC) | vC;
        else vC = portC;
      }
      if (maskD != 0) {
        if (RW == _ports::_Read_Write::Write) portD = (portD & ~maskD) | vD;
        else vD = portD;
      }
      
      uint8_t ret = 0;
      if (RW == _ports::_Read_Write::Read) {
        ret |= (v0 & _BV(D0::bit)) ? _BV(B0) : 0;
        ret |= (v1 & _BV(D1::bit)) ? _BV(B1) : 0;
        ret |= (v2 & _BV(D2::bit)) ? _BV(B2) : 0;
        ret |= (v3 & _BV(D3::bit)) ? _BV(B3) : 0;
        ret |= (v4 & _BV(D4::bit)) ? _BV(B4) : 0;
        ret |= (v5 & _BV(D5::bit)) ? _BV(B5) : 0;
        ret |= (v6 & _BV(D6::bit)) ? _BV(B6) : 0;
        ret |= (v7 & _BV(D7::bit)) ? _BV(B7) : 0;
      }
      return ret;
  }
  
  template <enum IOReg Reg,
            class D7, uint8_t B7,
            class D6, uint8_t B6,
            class D5, uint8_t B5,
            class D4, uint8_t B4,
            class D3, uint8_t B3,
            class D2, uint8_t B2,
            class D1, uint8_t B1,
            class D0, uint8_t B0,
            enum _Read_Write RW, typename V, typename P>
  inline static uint8_t _set_or_get_8_bits(const V& val) {
    static_assert(Reg == IOReg::DDRx ||
                  Reg == IOReg::PINx ||
                  Reg == IOReg::PORTx,
                  "IO Register is unknown.  Only Ddr, Pin and Port are supported.");
    switch (Reg) {
      case IOReg::DDRx:
        return _set_or_get_8_bits<D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0, RW>(DDRB, DDRC, DDRD, val);
      case IOReg::PORTx:
        return _set_or_get_8_bits<D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0, RW>(PORTB, PORTC, PORTD, val);
      case IOReg::PINx:
        return _set_or_get_8_bits<D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0, RW>(PINB, PINC, PIND, val);
    }
    return 0; // should never happen
  }
}

template <class D7, uint8_t B7,
          class D6, uint8_t B6,
          class D5, uint8_t B5,
          class D4, uint8_t B4,
          class D3, uint8_t B3,
          class D2, uint8_t B2,
          class D1, uint8_t B1,
          class D0, uint8_t B0,
          typename V, typename P>
inline static void set_8_bits(P& portB __attribute__ ((unused)), P& portC __attribute__ ((unused)), P& portD __attribute__ ((unused)), const V& val) {
  _ports::_set_or_get_8_bits<D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0, _ports::_Read_Write::Write>(portB, portC, portD, val);
}

template <class D7, uint8_t B7,
          class D6, uint8_t B6,
          class D5, uint8_t B5,
          class D4, uint8_t B4,
          class D3, uint8_t B3,
          class D2, uint8_t B2,
          class D1, uint8_t B1,
          class D0, uint8_t B0,
          typename V, typename P>
inline static uint8_t get_8_bits(P& portB __attribute__ ((unused)), P& portC __attribute__ ((unused)), P& portD __attribute__ ((unused)), const V& val) {
  return _ports::_set_or_get_8_bits<D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0, _ports::_Read_Write::Read>(portB, portC, portD, val);
}

template <enum _ports::IOReg IOReg,
          class D7, uint8_t B7,
          class D6, uint8_t B6,
          class D5, uint8_t B5,
          class D4, uint8_t B4,
          class D3, uint8_t B3,
          class D2, uint8_t B2,
          class D1, uint8_t B1,
          class D0, uint8_t B0,
          typename V, typename P>
inline static void set_8_bits(const V& val) {
  _ports::_set_or_get_8_bits<IOReg, D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0, _ports::_Read_Write::Write>(val);
}

template <enum _ports::IOReg IOReg,
          class D7, uint8_t B7,
          class D6, uint8_t B6,
          class D5, uint8_t B5,
          class D4, uint8_t B4,
          class D3, uint8_t B3,
          class D2, uint8_t B2,
          class D1, uint8_t B1,
          class D0, uint8_t B0,
          typename V, typename P>
inline static void get_8_bits(const V& val) {
  _ports::_set_or_get_8_bits<IOReg, D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0, _ports::_Read_Write::Read>(val);
}


template <enum _ports::IOReg IOReg,
          class D7,
          class D6,
          class D5,
          class D4,
          class D3,
          class D2,
          class D1,
          class D0,
          typename V, typename P>
inline static void set_8_byte(const V& val) {
  set_8_bits<IOReg, D7, 7, D6, 6, D5, 5, D4, 4, D3, 3, D2, 2, D1, 1, D0, 0>(val);
}

template <enum _ports::IOReg IOReg,
          uint8_t offset,
          class D3,
          class D2,
          class D1,
          class D0,
          typename V, typename P>
inline static void set_4_nibble(V& val) {
  set_8_byte<IOReg, PIN_UNUSED, -1, PIN_UNUSED, -1, PIN_UNUSED, -1, PIN_UNUSED, -1, D3, 3 + offset, D2, 2 + offset, D1, 1 + offset, D0, 0 + offset>(val);
}