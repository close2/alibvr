#pragma once
#include <avr/sfr_defs.h>


// SET_BIT(pin, reg, value)

// GET_BIT(pin, reg)

// PIN_PORT(pin, reg)

// bit_posX not >=0 && <8 → ignore this pin
// GET8(reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0)

// pin7 has bit_pos7 == 7, ...
// GET8_BYTE(reg, val, dest_pin7, dest_pin6, dest_pin5, dest_pin4, dest_pin3, dest_pin2, dest_pin1, dest_pin0)

// bit_posX not >=0 && <8 → ignore this pin
// SET8(reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0)

// pin7 has bit_pos7 == 7, ...
// SET8_BYTE(reg, val, dest_pin7, dest_pin6, dest_pin5, dest_pin4, dest_pin3, dest_pin2, dest_pin1, dest_pin0) SET8(reg, val, dest_pin7, 7, dest_pin6, 6, dest_pin5, 5, dest_pin4, 4, dest_pin3, 3, dest_pin2, 2, dest_pin1, 1, dest_pin0, 0)

// bit_posX not >=0 && <8 → ignore this pin
// pin0 has bit_pos0 == 0 + offset, ...
// SET4_NIBBLE(reg, val, offset, dest_pin3, dest_pin2, dest_pin1, dest_pin0) SET4(reg, val, dest_pin3, 3 + offset, dest_pin2, 2 + offset, dest_pin1, 1 + offset, dest_pin0, 0 + offset)

// PIN layout is compatible to arduino / arduino lite
#define PIN_0_PORT D
#define PIN_0_BIT 0
#define PIN_1_PORT D
#define PIN_1_BIT 1
#define PIN_2_PORT D
#define PIN_2_BIT 2
#define PIN_3_PORT D
#define PIN_3_BIT 3
#define PIN_4_PORT D
#define PIN_4_BIT  4
#define PIN_5_PORT D
#define PIN_5_BIT 5
#define PIN_6_PORT D
#define PIN_6_BIT 6
#define PIN_7_PORT D
#define PIN_7_BIT 7
#define PIN_8_PORT B
#define PIN_8_BIT 0
#define PIN_9_PORT B
#define PIN_9_BIT 1
#define PIN_10_PORT B
#define PIN_10_BIT 2
#define PIN_11_PORT B
#define PIN_11_BIT 3
#define PIN_12_PORT B
#define PIN_12_BIT 4
#define PIN_13_PORT B
#define PIN_13_BIT 5
#define PIN_14_PORT C
#define PIN_14_BIT 0
#define PIN_15_PORT C
#define PIN_15_BIT 1
#define PIN_16_PORT C
#define PIN_16_BIT 2
#define PIN_17_PORT C
#define PIN_17_BIT 3
#define PIN_18_PORT C
#define PIN_18_BIT 4
#define PIN_19_PORT C
#define PIN_19_BIT 5
#define PIN_20_PORT B
#define PIN_20_BIT 6
#define PIN_21_PORT B
#define PIN_21_BIT 7

//                  +-\/-+
//            PC6  1|    |28  PC5 (AI 5/*D19)
//      (D 0) PD0  2|    |27  PC4 (AI 4/*D18)
//      (D 1) PD1  3|    |26  PC3 (AI 3/*D17)
//      (D 2) PD2  4|    |25  PC2 (AI 2/*D16)
// PWM+ (D 3) PD3  5|    |24  PC1 (AI 1/*D15)
//      (D 4) PD4  6|    |23  PC0 (AI 0/*D14)
//            VCC  7|    |22  GND
//            GND  8|    |21  AREF
//     *(D20) PB6  9|    |20  AVCC
//     *(D21) PB7 10|    |19  PB5 (D 13)
// PWM+ (D 5) PD5 11|    |18  PB4 (D 12)
// PWM+ (D 6) PD6 12|    |17  PB3 (D 11) PWM
//      (D 7) PD7 13|    |16  PB2 (D 10) PWM
//      (D 8) PB0 14|    |15  PB1 (D 9) PWM
//                  +----+

#define PIN_DIP_1_PORT C
#define PIN_DIP_1_BIT 6
#define PIN_DIP_2 PIN_0
#define PIN_DIP_3 PIN_1
#define PIN_DIP_4 PIN_2
#define PIN_DIP_5 PIN_3
#define PIN_DIP_6 PIN_4
#define PIN_DIP_9 PIN_20
#define PIN_DIP_10 PIN_21
#define PIN_DIP_11 PIN_5
#define PIN_DIP_12 PIN_6
#define PIN_DIP_13 PIN_7
#define PIN_DIP_14 PIN_8
#define PIN_DIP_15 PIN_9
#define PIN_DIP_16 PIN_10
#define PIN_DIP_17 PIN_11
#define PIN_DIP_18 PIN_12
#define PIN_DIP_19 PIN_13
#define PIN_DIP_23 PIN_14
#define PIN_DIP_24 PIN_15
#define PIN_DIP_25 PIN_16
#define PIN_DIP_26 PIN_17
#define PIN_DIP_27 PIN_18
#define PIN_DIP_28 PIN_19

#define ___SET_BIT(port, bit, reg, value) do { if ((value)) reg##port |= _BV(bit); else reg##port &= ~_BV(bit); } while(0)
#define __SET_BIT(port, bit, reg, value) ___SET_BIT(port, bit, reg, value)
#define _SET_BIT(pin, reg, value) __SET_BIT(pin##_PORT, pin##_BIT, reg, value)
#define SET_BIT(pin, reg, value) _SET_BIT(pin, reg, value)

#define ___GET_BIT(port, bit, reg) (reg##port & _BV(bit))
#define __GET_BIT(port, bit, reg) ___GET_BIT(port, bit, reg)
#define _GET_BIT(pin, reg) __GET_BIT(pin##_PORT, pin##_BIT, reg)
#define GET_BIT(pin, reg) _GET_BIT(pin, reg)

#define ___PIN_PORT(port, reg) (reg##port)
#define __PIN_PORT(port, reg) ___PIN_PORT(port, reg)
#define _PIN_PORT(pin, reg) __PIN_PORT(pin##_PORT, reg)
#define PIN_PORT(pin, reg) _PIN_PORT(pin, reg)

#define ___PIN_BIT(bit, reg) (reg##bit)
#define __PIN_BIT(bit, reg) ___PIN_BIT(bit, reg)
#define _PIN_BIT(pin, reg) __PIN_BIT(pin##_BIT, reg)
#define PIN_BIT(pin, reg) _PIN_BIT(pin, reg)

namespace _macros {
  enum portNames {
    B,
    C,
    D
  };
  enum readWrite {
    Read,
    Write
  };
  
  // V .. value
  // P .. port variables (in write mode Dest, in read mode Src)
  // Bn .. bit in value (in write mode Src, in read mode Dest)
  // Dn .. bit in Px (depending on EPn) (in write mode Dest, in read mode Src)
  // EPn .. enum B, C or D for port (in write mode Dest-port, in read mode Src-port)
  template <enum portNames EP0, uint8_t D0, uint8_t B0,
            enum portNames EP1, uint8_t D1, uint8_t B1,
            enum portNames EP2, uint8_t D2, uint8_t B2,
            enum portNames EP3, uint8_t D3, uint8_t B3,
            enum portNames EP4, uint8_t D4, uint8_t B4,
            enum portNames EP5, uint8_t D5, uint8_t B5,
            enum portNames EP6, uint8_t D6, uint8_t B6,
            enum portNames EP7, uint8_t D7, uint8_t B7,
            enum readWrite RW, typename V, typename P>
  inline static void _setOrGet8Bits(P& portB __attribute__ ((unused)), P& portC __attribute__ ((unused)), P& portD __attribute__ ((unused)), V& val) {
      uint8_t vB __attribute__ ((unused)) = 0;
      uint8_t vC __attribute__ ((unused)) = 0;
      uint8_t vD __attribute__ ((unused)) = 0;
      uint8_t maskB = 0;
      uint8_t maskC = 0;
      uint8_t maskD = 0;
      
      const bool d0isB = EP0 == _macros::B; const bool d0isC = EP0 == _macros::C;
      const bool d1isB = EP1 == _macros::B; const bool d1isC = EP1 == _macros::C;
      const bool d2isB = EP2 == _macros::B; const bool d2isC = EP2 == _macros::C;
      const bool d3isB = EP3 == _macros::B; const bool d3isC = EP3 == _macros::C;
      const bool d4isB = EP4 == _macros::B; const bool d4isC = EP4 == _macros::C;
      const bool d5isB = EP5 == _macros::B; const bool d5isC = EP5 == _macros::C;
      const bool d6isB = EP6 == _macros::B; const bool d6isC = EP6 == _macros::C;
      const bool d7isB = EP7 == _macros::B; const bool d7isC = EP7 == _macros::C;

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

      if (D0 >= 0 && D0 < 8 && B0 >= 0 && B0 < 8) {
        mask0 |= 1 << D0;
        if (RW == _macros::Write) v0 |= val & _BV(B0);
      }
      if (D1 >= 0 && D1 < 8 && B1 >= 0 && B1 < 8) {
        mask1 |= 1 << D1;
        if (RW == _macros::Write) v1 |= val & _BV(B1);
      }
      if (D2 >= 0 && D2 < 8 && B2 >= 0 && B2 < 8) {
        mask2 |= 1 << D2;
        if (RW == _macros::Write) v2 |= val & _BV(B2);
      }
      if (D3 >= 0 && D3 < 8 && B3 >= 0 && B3 < 8) {
        mask3 |= 1 << D3;
        if (RW == _macros::Write) v3 |= val & _BV(B3);
      }
      if (D4 >= 0 && D4 < 8 && B4 >= 0 && B4 < 8) {
        mask4 |= 1 << D4;
        if (RW == _macros::Write) v4 |= val & _BV(B4);
      }
      if (D5 >= 0 && D5 < 8 && B5 >= 0 && B5 < 8) {
        mask5 |= 1 << D5;
        if (RW == _macros::Write) v5 |= val & _BV(B5);
      }
      if (D6 >= 0 && D6 < 8 && B6 >= 0 && B6 < 8) {
        mask6 |= 1 << D6;
        if (RW == _macros::Write) v6 |= val & _BV(B6);
      }
      if (D7 >= 0 && D7 < 8 && B7 >= 0 && B7 < 8) {
        mask7 |= 1 << D7;
        if (RW == _macros::Write) v7 |= val & _BV(B7);
      }
  
      if (maskB != 0) {
        if (RW == _macros::Write) portB = (portB & ~maskB) | vB;
        else vB = portB;
      }
      if (maskC != 0) {
        if (RW == _macros::Write) portC = (portC & ~maskC) | vC;
        else vC = portC;
      }
      if (maskD != 0) {
        if (RW == _macros::Write) portD = (portD & ~maskD) | vD;
        else vD = portD;
      }
      
      if (RW == _macros::Read) {
        val = 0;
        val |= (v0 & _BV(D0)) ? _BV(B0) : 0;
        val |= (v1 & _BV(D1)) ? _BV(B1) : 0;
        val |= (v2 & _BV(D2)) ? _BV(B2) : 0;
        val |= (v3 & _BV(D3)) ? _BV(B3) : 0;
        val |= (v4 & _BV(D4)) ? _BV(B4) : 0;
        val |= (v5 & _BV(D5)) ? _BV(B5) : 0;
        val |= (v6 & _BV(D6)) ? _BV(B6) : 0;
        val |= (v7 & _BV(D7)) ? _BV(B7) : 0;
      }
  }
  // a small wrapper function for _setOrGet8Bits, to avoid problems with the const of value
  // in write mode.
  template <enum portNames EP0, uint8_t D0, uint8_t B0,
            enum portNames EP1, uint8_t D1, uint8_t B1,
            enum portNames EP2, uint8_t D2, uint8_t B2,
            enum portNames EP3, uint8_t D3, uint8_t B3,
            enum portNames EP4, uint8_t D4, uint8_t B4,
            enum portNames EP5, uint8_t D5, uint8_t B5,
            enum portNames EP6, uint8_t D6, uint8_t B6,
            enum portNames EP7, uint8_t D7, uint8_t B7,
            typename V, typename P>
  inline static void set8Bits(P& portB __attribute__ ((unused)), P& portC __attribute__ ((unused)), P& portD __attribute__ ((unused)), V val) {
    _macros::_setOrGet8Bits<EP0, D0, B0,
                       EP1, D1, B1,
                       EP2, D2, B2,
                       EP3, D3, B3,
                       EP4, D4, B4,
                       EP5, D5, B5,
                       EP6, D6, B6,
                       EP7, D7, B7,
                       _macros::Write>(portB, portC, portD, val);
  }

  // a small wrapper function for _setOrGet8Bits, to avoid problems with the const of value
  // in write mode.
  template <enum portNames EP0, uint8_t D0, uint8_t B0,
            enum portNames EP1, uint8_t D1, uint8_t B1,
            enum portNames EP2, uint8_t D2, uint8_t B2,
            enum portNames EP3, uint8_t D3, uint8_t B3,
            enum portNames EP4, uint8_t D4, uint8_t B4,
            enum portNames EP5, uint8_t D5, uint8_t B5,
            enum portNames EP6, uint8_t D6, uint8_t B6,
            enum portNames EP7, uint8_t D7, uint8_t B7,
            typename V, typename P>
  inline static void get8Bits(P& portB __attribute__ ((unused)), P& portC __attribute__ ((unused)), P& portD __attribute__ ((unused)), V& val) {
    _macros::_setOrGet8Bits<EP0, D0, B0,
                       EP1, D1, B1,
                       EP2, D2, B2,
                       EP3, D3, B3,
                       EP4, D4, B4,
                       EP5, D5, B5,
                       EP6, D6, B6,
                       EP7, D7, B7,
                       _macros::Read>(portB, portC, portD, val);
  }

}
#define ___GET_OR_SET8(get_set_fun, reg_b, reg_c, reg_d, val, dest_pin7_port, dest_pin7_bit, bit_pos7, dest_pin6_port, dest_pin6_bit, bit_pos6, dest_pin5_port, dest_pin5_bit, bit_pos5, dest_pin4_port, dest_pin4_bit, bit_pos4, dest_pin3_port, dest_pin3_bit, bit_pos3, dest_pin2_port, dest_pin2_bit, bit_pos2, dest_pin1_port, dest_pin1_bit, bit_pos1, dest_pin0_port, dest_pin0_bit, bit_pos0) \
  _macros::get_set_fun<dest_pin0_port, dest_pin0_bit, bit_pos0, \
                       dest_pin1_port, dest_pin1_bit, bit_pos1, \
                       dest_pin2_port, dest_pin2_bit, bit_pos2, \
                       dest_pin3_port, dest_pin3_bit, bit_pos3, \
                       dest_pin4_port, dest_pin4_bit, bit_pos4, \
                       dest_pin5_port, dest_pin5_bit, bit_pos5, \
                       dest_pin6_port, dest_pin6_bit, bit_pos6, \
                       dest_pin7_port, dest_pin7_bit, bit_pos7 \
                       >(reg_b, reg_c, reg_d, val)
#define __GET_OR_SET8(get_set_fun, reg_b, reg_c, reg_d, val, dest_pin7_port, dest_pin7_bit, bit_pos7, dest_pin6_port, dest_pin6_bit, bit_pos6, dest_pin5_port, dest_pin5_bit, bit_pos5, dest_pin4_port, dest_pin4_bit, bit_pos4, dest_pin3_port, dest_pin3_bit, bit_pos3, dest_pin2_port, dest_pin2_bit, bit_pos2, dest_pin1_port, dest_pin1_bit, bit_pos1, dest_pin0_port, dest_pin0_bit, bit_pos0) \
        ___GET_OR_SET8(get_set_fun, reg_b, reg_c, reg_d, val, dest_pin7_port, dest_pin7_bit, bit_pos7, dest_pin6_port, dest_pin6_bit, bit_pos6, dest_pin5_port, dest_pin5_bit, bit_pos5, dest_pin4_port, dest_pin4_bit, bit_pos4, dest_pin3_port, dest_pin3_bit, bit_pos3, dest_pin2_port, dest_pin2_bit, bit_pos2, dest_pin1_port, dest_pin1_bit, bit_pos1, dest_pin0_port, dest_pin0_bit, bit_pos0)
#define _GET_OR_SET8(get_set_fun, reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0) \
        __GET_OR_SET8(get_set_fun, reg##B, reg##C, reg##D, val, _macros::dest_pin7##_PORT, dest_pin7##_BIT, bit_pos7, _macros::dest_pin6##_PORT, dest_pin6##_BIT, bit_pos6, _macros::dest_pin5##_PORT, dest_pin5##_BIT, bit_pos5, _macros::dest_pin4##_PORT, dest_pin4##_BIT, bit_pos4, _macros::dest_pin3##_PORT, dest_pin3##_BIT, bit_pos3, _macros::dest_pin2##_PORT, dest_pin2##_BIT, bit_pos2, _macros::dest_pin1##_PORT, dest_pin1##_BIT, bit_pos1, _macros::dest_pin0##_PORT, dest_pin0##_BIT, bit_pos0)
#define SET8(reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0) \
        _GET_OR_SET8(set8Bits, reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0)
 
#define GET8(reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0) \
        _GET_OR_SET8(get8Bits, reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0)

        
#define GET8_BYTE(reg, val, dest_pin7, dest_pin6, dest_pin5, dest_pin4, dest_pin3, dest_pin2, dest_pin1, dest_pin0) GET8(reg, val, dest_pin7, 7, dest_pin6, 6, dest_pin5, 5, dest_pin4, 4, dest_pin3, 3, dest_pin2, 2, dest_pin1, 1, dest_pin0, 0)

#define SET8_BYTE(reg, val, dest_pin7, dest_pin6, dest_pin5, dest_pin4, dest_pin3, dest_pin2, dest_pin1, dest_pin0) SET8(reg, val, dest_pin7, 7, dest_pin6, 6, dest_pin5, 5, dest_pin4, 4, dest_pin3, 3, dest_pin2, 2, dest_pin1, 1, dest_pin0, 0)
#define SET4_NIBBLE(reg, val, offset, dest_pin3, dest_pin2, dest_pin1, dest_pin0) SET8(reg, val, dest_pin0, -1, dest_pin0, -1, dest_pin0, -1, dest_pin0, -1, dest_pin3, 3 + offset, dest_pin2, 2 + offset, dest_pin1, 1 + offset, dest_pin0, 0 + offset)
