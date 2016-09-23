#define F_CPU 8000000
#include "ports.h"
#include "irqs.h"
#include "adc.h"

//«ADC_EX»
typedef PIN_C2 Led;
typedef PIN_ADC0 AnalogIn;

void f(const uint16_t& result) {
  Led::PORT = ((uint8_t)result) > 0x0F;
}

typedef adc::Adc<adc::Ref::V1_1, AnalogIn, adc::Mode::FreeRunning, f> MyAdc;
#define NEW_ADC MyAdc
#include REGISTER_ADC

int main(void) {
  Led::DDR = ports::DataDirection::Output;   // Put Led pin into output mode.
  Led::PORT = 1;  // Set Led pin output to high. (I.e. turn it on.)
  
  MyAdc::init();
  MyAdc::start_adc_8bit();
/*¤*/
  
  
  for (;;);
  return 0;
}
#include REGISTER_IRQS
