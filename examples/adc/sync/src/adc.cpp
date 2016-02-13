#define F_CPU 8000000UL

#include <util/delay.h>

#include "macros.h"
#include "adc.h"

// First define the Adc.
// We want a single conversion, whenever we start a conversion.
// Use the internal 1.1V voltage as reference.
// Measure the voltage on Pin ADC4.
typedef Adc<_adc::Mode::SingleConversion, _adc::Ref::V1_1, PIN_ADC4> Adc4;

// We also want to measure the voltage on Pin ADC5
typedef Adc<_adc::Mode::SingleConversion, _adc::Ref::V1_1, PIN_ADC5> Adc5;

typedef PIN_D6 Led;

__attribute__ ((OS_main)) int main(void) {
  // put Led pin into output mode.
  set_bit<Led::ddr>(1);
  
  for (;;) {
    Adc4::init();
    auto adc4 = Adc4::adc_8bit();
    Adc5::init();
    auto adc5 = Adc5::adc_8bit();
    
    set_bit<Led::port>(adc4 < adc5);
    _delay_ms(300);
  }
  return 0;
}