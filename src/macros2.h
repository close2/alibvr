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

#define _SET8_SET_BIT(n, set_zero, reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0) \
  do { \
    if (val & _BV(n)) { \
        if (bit_pos0 == n) SET_BIT(dest_pin0, reg, 1); \
        if (bit_pos1 == n) SET_BIT(dest_pin1, reg, 1); \
        if (bit_pos2 == n) SET_BIT(dest_pin2, reg, 1); \
        if (bit_pos3 == n) SET_BIT(dest_pin3, reg, 1); \
        if (bit_pos4 == n) SET_BIT(dest_pin4, reg, 1); \
        if (bit_pos5 == n) SET_BIT(dest_pin5, reg, 1); \
        if (bit_pos6 == n) SET_BIT(dest_pin6, reg, 1); \
        if (bit_pos7 == n) SET_BIT(dest_pin7, reg, 1); \
    } else if (set_zero) { \
        if (bit_pos0 == n) SET_BIT(dest_pin0, reg, 0); \
        if (bit_pos1 == n) SET_BIT(dest_pin1, reg, 0); \
        if (bit_pos2 == n) SET_BIT(dest_pin2, reg, 0); \
        if (bit_pos3 == n) SET_BIT(dest_pin3, reg, 0); \
        if (bit_pos4 == n) SET_BIT(dest_pin4, reg, 0); \
        if (bit_pos5 == n) SET_BIT(dest_pin5, reg, 0); \
        if (bit_pos6 == n) SET_BIT(dest_pin6, reg, 0); \
        if (bit_pos7 == n) SET_BIT(dest_pin7, reg, 0); \
    } \
  } while (0)

#define GET8(reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0) \
  do { \
    uint8_t vB __attribute__ ((unused)); \
    uint8_t vC __attribute__ ((unused)); \
    uint8_t vD __attribute__ ((unused)); \
    uint8_t maskB = 0; \
    uint8_t maskC = 0; \
    uint8_t maskD = 0; \
    \
    if (bit_pos7 >=0 && bit_pos7 < 8) SET_BIT(dest_pin7, mask, 1); \
    if (bit_pos6 >=0 && bit_pos7 < 8) SET_BIT(dest_pin6, mask, 1); \
    if (bit_pos5 >=0 && bit_pos7 < 8) SET_BIT(dest_pin5, mask, 1); \
    if (bit_pos4 >=0 && bit_pos7 < 8) SET_BIT(dest_pin4, mask, 1); \
    if (bit_pos3 >=0 && bit_pos7 < 8) SET_BIT(dest_pin3, mask, 1); \
    if (bit_pos2 >=0 && bit_pos7 < 8) SET_BIT(dest_pin2, mask, 1); \
    if (bit_pos1 >=0 && bit_pos7 < 8) SET_BIT(dest_pin1, mask, 1); \
    if (bit_pos0 >=0 && bit_pos7 < 8) SET_BIT(dest_pin0, mask, 1); \
    \
    if (maskB != 0) { \
      vB = reg##B; \
    } \
    if (maskC != 0) { \
      vC = reg##C; \
    } \
    if (maskD != 0) { \
      vD = reg##D; \
    } \
    \
    val = 0; \
    if (bit_pos7 >=0 && bit_pos7 < 8 && GET_BIT(dest_pin7, v)) val |= _BV(bit_pos7); \
    if (bit_pos6 >=0 && bit_pos7 < 8 && GET_BIT(dest_pin6, v)) val |= _BV(bit_pos6); \
    if (bit_pos5 >=0 && bit_pos7 < 8 && GET_BIT(dest_pin5, v)) val |= _BV(bit_pos5); \
    if (bit_pos4 >=0 && bit_pos7 < 8 && GET_BIT(dest_pin4, v)) val |= _BV(bit_pos4); \
    if (bit_pos3 >=0 && bit_pos7 < 8 && GET_BIT(dest_pin3, v)) val |= _BV(bit_pos3); \
    if (bit_pos2 >=0 && bit_pos7 < 8 && GET_BIT(dest_pin2, v)) val |= _BV(bit_pos2); \
    if (bit_pos1 >=0 && bit_pos7 < 8 && GET_BIT(dest_pin1, v)) val |= _BV(bit_pos1); \
    if (bit_pos0 >=0 && bit_pos7 < 8 && GET_BIT(dest_pin0, v)) val |= _BV(bit_pos0); \
  } while(0)

#define GET8_BYTE(reg, val, dest_pin7, dest_pin6, dest_pin5, dest_pin4, dest_pin3, dest_pin2, dest_pin1, dest_pin0) GET8(reg, val, dest_pin7, 7, dest_pin6, 6, dest_pin5, 5, dest_pin4, 4, dest_pin3, 3, dest_pin2, 2, dest_pin1, 1, dest_pin0, 0)

#define SET8(reg, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0) \
  do { \
    uint8_t vB = 0; \
    uint8_t vC = 0; \
    uint8_t vD = 0; \
    uint8_t maskB = 0; \
    uint8_t maskC = 0; \
    uint8_t maskD = 0; \
    \
    _SET8_SET_BIT(0, 0, v, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0); \
    _SET8_SET_BIT(1, 0, v, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0); \
    _SET8_SET_BIT(2, 0, v, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0); \
    _SET8_SET_BIT(3, 0, v, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0); \
    _SET8_SET_BIT(4, 0, v, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0); \
    _SET8_SET_BIT(5, 0, v, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0); \
    _SET8_SET_BIT(6, 0, v, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0); \
    _SET8_SET_BIT(7, 0, v, val, dest_pin7, bit_pos7, dest_pin6, bit_pos6, dest_pin5, bit_pos5, dest_pin4, bit_pos4, dest_pin3, bit_pos3, dest_pin2, bit_pos2, dest_pin1, bit_pos1, dest_pin0, bit_pos0); \
    \
    if (bit_pos0 >= 0 && bit_pos0 < 8) SET_BIT(dest_pin0, mask, 1); \
    if (bit_pos1 >= 0 && bit_pos0 < 8) SET_BIT(dest_pin1, mask, 1); \
    if (bit_pos2 >= 0 && bit_pos0 < 8) SET_BIT(dest_pin2, mask, 1); \
    if (bit_pos3 >= 0 && bit_pos0 < 8) SET_BIT(dest_pin3, mask, 1); \
    if (bit_pos4 >= 0 && bit_pos0 < 8) SET_BIT(dest_pin4, mask, 1); \
    if (bit_pos5 >= 0 && bit_pos0 < 8) SET_BIT(dest_pin5, mask, 1); \
    if (bit_pos6 >= 0 && bit_pos0 < 8) SET_BIT(dest_pin6, mask, 1); \
    if (bit_pos7 >= 0 && bit_pos0 < 8) SET_BIT(dest_pin7, mask, 1); \
    \
    if (maskB != 0) { \
      if (maskB == 0xFF) reg##B = vB; \
      else { uint8_t b = reg##B; \
             reg##B = (b & ~maskB) | vB; \
      } \
    } \
    if (maskC != 0) { \
      if (maskC == 0xFF) reg##C = vC; \
      else { uint8_t b = reg##C; \
             reg##C = (b & ~maskC) | vC; \
      } \
    } \
    if (maskD != 0) { \
      if (maskD == 0xFF) reg##D = vD; \
      else { uint8_t b = reg##D; \
             reg##D = (b & ~maskD) | vD; \
      } \
    } \
  } while(0)

#define SET8_BYTE(reg, val, dest_pin7, dest_pin6, dest_pin5, dest_pin4, dest_pin3, dest_pin2, dest_pin1, dest_pin0) SET8(reg, val, dest_pin7, 7, dest_pin6, 6, dest_pin5, 5, dest_pin4, 4, dest_pin3, 3, dest_pin2, 2, dest_pin1, 1, dest_pin0, 0)
#define SET4_NIBBLE(reg, val, offset, dest_pin3, dest_pin2, dest_pin1, dest_pin0) SET8(reg, val, dest_pin0, -1, dest_pin0, -1, dest_pin0, -1, dest_pin0, -1, dest_pin3, 3 + offset, dest_pin2, 2 + offset, dest_pin1, 1 + offset, dest_pin0, 0 + offset)
