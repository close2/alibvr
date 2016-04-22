#define F_CPU 8000000UL

#include <util/delay.h>

#include "ports.h"
#include "adc.h"

// First define the Adc.
// We want a single conversion, whenever we start a conversion.
// Use the internal 1.1V voltage as reference.
typedef Adc<_adc::Ref::V1_1> AdcV1_1;

typedef PIN_D6 Led;

__attribute__ ((OS_main)) int main(void) {
  // put Led pin into output mode.
  Led::DDR = 1;
  
  for (;;) {
    // Measure the voltage on Pin ADC4.
    AdcV1_1::init<PIN_ADC4>();
    auto adc4 = AdcV1_1::adc_8bit();
    // We also want to measure the voltage on Pin ADC5
    AdcV1_1::init<PIN_ADC5>();
    auto adc5 = AdcV1_1::adc_8bit();
    
    Led::PORT = (adc4 < adc5);
    _delay_ms(30);
  }
  return 0;
}