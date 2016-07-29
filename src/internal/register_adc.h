#pragma push_macro("NEW_IRQ_TASK")

#define NEW_IRQ_TASK NEW_ADC::handle
#include "register_irq_task_ADC.h"

#ifndef _ADC_REGISTERED
#  define _ADC_REGISTERED
  // This functions makes sure that an IRQ handler is registered,
  // when adc with noise reduction is used.
  // Using adc with noise reduction without having an IRQ handler
  // would reset the avr when the adc has finished.
  // By defining the function here (even though it doesn't do anything)
  // the compiler will make sure that this header has been included
  // or otherwise display a "meaningful" error.
namespace ALIBVR_NAMESPACE_ADC {
  void irq_handler_for_adc_must_be_registered_for_noise_reduction() {}
}
#endif

#pragma pop_macro("NEW_IRQ_TASK")
