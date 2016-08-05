#define F_CPU 8000000

#include "adc.h"
#include "irqs.h"
#include "ports.h"

using namespace adc;

void f(const uint16_t& result) {
  PIN_B4::PORT = result > 0x0F;
}

// «ADC_IRQ_REG»
typedef Adc<Ref::V1_1, Input::Unset, Mode::SingleConversion, f> MyAdc;
#define NEW_ADC MyAdc
#include REGISTER_ADC
/*¤*/

int main() {
  PIN_B4::DDR = ports::DataDirection::Output;
  
  MyAdc::init<PIN_ADC0>();
  sei();
  
  MyAdc::start_adc_10bit();
  
  for(;;);
}

#include REGISTER_IRQS