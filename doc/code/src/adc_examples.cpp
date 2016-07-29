#define F_CPU 8000000

#include "adc.h"
#include "irqs.h"
#include "ports.h"

using namespace adc;

void f(const uint16_t& result) {
  PIN_B4::PORT = result > 0x0F;
}

void f2(const uint16_t& result) {
  PIN_B5::PORT = result > 0x0F;
}

// «ADC_IRQ_REG»
typedef Adc<Ref::V1_1, Input::Unset, Mode::SingleConversion, f> MyAdc;
#define NEW_ADC MyAdc
#include REGISTER_ADC
#undef NEW_ADC
  
typedef Adc<Ref::V1_1, Input::Unset, Mode::SingleConversion, f2> MyAdc2;
#define NEW_ADC MyAdc2
#include REGISTER_ADC
/*¤*/
#undef NEW_ADC

// «ADC_NOISE_RED1»
typedef Adc<Ref::V1_1> NoiseRedAdc;
#define NEW_ADC NoiseRedAdc
#include REGISTER_ADC
/*¤*/


int main() {
  PIN_B4::DDR = ports::DataDirection::Output;
  PIN_B5::DDR = ports::DataDirection::Output;
  
  uint8_t adc3;
  uint8_t adc4;
  // «ADC_2_INPUTS1[^  ,]»
  typedef Adc<Ref::V1_1, Input::Unset> Adc1_1;
  Adc1_1::init<PIN_ADC3>();
  adc3 = Adc1_1::adc_8bit();
  Adc1_1::init<PIN_ADC4>();
  adc4 = Adc1_1::adc_8bit();
  /*¤*/
  // Use variables to avoid warnings.
  if (adc3 == adc4) PIN_B4::PORT = 1;
  
  // «ADC_2_INPUTS2[^  ,]»
  typedef Adc<Ref::V1_1, PIN_ADC3> AdcAdc3;
  typedef Adc<Ref::V1_1, PIN_ADC4> AdcAdc4;
  AdcAdc3::init();
  adc3 = AdcAdc3::adc_8bit();
  AdcAdc4::init();
  adc4 = AdcAdc4::adc_8bit();
  /*¤*/
  
  // «ADC_SHARED[^  ,]»
  AdcAdc3::init();
  AdcAdc4::init();
  // !!! THIS WON'T WORK !!!
  // AdcAdc3::adc_8bit() will use PIN_ADC4 as input!
  // AdcAdc4::init() switched the adc subsystem to input PIN_ADC4.
  adc3 = AdcAdc3::adc_8bit();
  adc4 = AdcAdc4::adc_8bit();
  /*¤*/
  
  
  // «ADC_SYNC[^  ,]»
  MyAdc::init<PIN_ADC0>();
  uint8_t adc1 = MyAdc::adc_8bit();
  
  MyAdc2::init<PIN_ADC1>();
  uint16_t adc2 = MyAdc2::adc_10bit();
  /*¤*/
  
  // «ADC_NOISE_RED2[^  ,]»
  NoiseRedAdc::init<PIN_ADC3>();
  uint16_t adcNoiseRed = NoiseRedAdc::adc_10bit<1>();
  /*¤*/
  
  // Use variables to avoid warnings.
  if (adc1 == adc2) PIN_ADC3::DDR = 1;
  if (adcNoiseRed == 0x0F) PIN_ADC3::PORT = 1;
  
  for(;;);
}


// «ADC_NOISE_RED3»
#include REGISTER_IRQS
/*¤*/
