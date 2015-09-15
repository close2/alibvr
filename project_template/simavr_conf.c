#include </usr/include/simavr/avr/avr_mcu_section.h>
#include <avr/sfr_defs.h>
#include <avr/io.h>
AVR_MCU(1000000UL, "atmega328");

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
//
// typedef Lcd<PIN_17, PIN_16, PIN_15, PIN_14, PIN_19, PIN_18> LCD;

const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
  { AVR_MCU_VCD_SYMBOL("D4"),        .mask = 0b001000 /* PIN_17 == C3 */, .what = (void*)&PORTC, },  
  { AVR_MCU_VCD_SYMBOL("D5"),        .mask = 0b000100 /* PIN_16 == C2 */, .what = (void*)&PORTC, },  
  { AVR_MCU_VCD_SYMBOL("D6"),        .mask = 0b000010 /* PIN_15 == C1 */, .what = (void*)&PORTC, },  
  { AVR_MCU_VCD_SYMBOL("D7"),        .mask = 0b000001 /* PIN_14 == C0 */, .what = (void*)&PORTC, },  
  { AVR_MCU_VCD_SYMBOL("Enable"),    .mask = 0b100000 /* PIN_19 == C5 */, .what = (void*)&PORTC, },  
  { AVR_MCU_VCD_SYMBOL("RegSelect"), .mask = 0b010000 /* PIN_18 == C4 */, .what = (void*)&PORTC, }, 
};