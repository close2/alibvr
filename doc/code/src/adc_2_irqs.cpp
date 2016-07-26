#define F_CPU 8000000

#include "adc.h"
#include "irqs.h"
#include "ports.h"

using namespace adc;


void f(const uint16_t& result) {
  PIN_B4::PORT = result > 0x0F;
}

void f2(const uint16_t& result) {
  PIN_B3::PORT = result < 0x0F;
}


/*
 * THIS IS PROBABLY NOT WHAT YOU WOULD WANT IN YOUR PROGRAM!
 * 
 * THE ADC SUBSYSTEM WILL CALL BOTH f AND f2 AFTER EVERY CONVERSION!
 */


// «ADC_IRQ_REG»
typedef Adc<Ref::V1_1, Input::Unset, Mode::SingleConversion, f> MyAdc;
#define NEW_ADC MyAdc
#include REGISTER_ADC
/*¤*/

typedef Adc<Ref::AVcc, PIN_ADC3, Mode::SingleConversion, f2> MyAdc2;
#define NEW_ADC MyAdc
#include REGISTER_ADC

int main() {
  PIN_B3::DDR = ports::DataDirection::Output;
  PIN_B4::DDR = ports::DataDirection::Output;
  
  
  // If there was only one Adc we could move the init outside the loop,
  // but in this case the init() switches the input
  for(;;) {
    MyAdc::init<PIN_ADC0>();
    MyAdc::start_adc_10bit();
    
    MyAdc::busy_wait_adc_finished();
    // After the conversion has finished both! f and f2 will be executed.
    
    MyAdc2::init();
    MyAdc2::start_adc_8bit();
    MyAdc::busy_wait_adc_finished();
    // After the conversion has finished both! f and f2 will be executed.
  }
}

#include REGISTER_IRQS