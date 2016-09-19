#pragma once

#include <avr/interrupt.h>
#include "irqs.h"

namespace _irqs {
  inline uint8_t you_must_include_register_irqs_h() { return 0; }
}

#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_INT0)
ISR(INT0_vect) {
  _irqs::execIrqTasks<0, _irqs::Irq::IRQ_INT0>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_INT1)
ISR(INT1_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_INT1>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_PCINT0)
ISR(PCINT0_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_PCINT0>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_PCINT1)
ISR(PCINT1_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_PCINT1>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_PCINT2)
ISR(PCINT2_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_PCINT2>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_WDT)
ISR(WDT_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_WDT>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER2_COMPA)
ISR(TIMER2_COMPA_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TIMER2_COMPA>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER2_COMPB)
ISR(TIMER2_COMPB_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TIMER2_COMPB>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER2_OVF)
ISR(TIMER2_OVF_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TIMER2_OVF>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER1_CAPT)
ISR(TIMER1_CAPT_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TIMER1_CAPT>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER1_COMPA)
ISR(TIMER1_COMPA_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TIMER1_COMPA>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER1_COMPB)
ISR(TIMER1_COMPB_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TIMER1_COMPB>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER1_OVF)
ISR(TIMER1_OVF_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TIMER1_OVF>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER0_COMPA)
ISR(TIMER0_COMPA_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TIMER0_COMPA>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER0_COMPB)
ISR(TIMER0_COMPB_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TIMER0_COMPB>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TIMER0_OVF)
ISR(TIMER0_OVF_vect) {
  _irqs::execIrqTasks<0, _irqs::Irq::IRQ_TIMER0_OVF>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_SPI_STC)
ISR(SPI_STC_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_SPI_STC>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_USART_RX)
ISR(USART_RX_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_USART_RX>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_USART_UDRE)
ISR(USART_UDRE_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_USART_UDRE>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_USART_TX)
ISR(USART_TX_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_USART_TX>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_ADC)
ISR(ADC_vect) {
  _irqs::execIrqTasks<0, _irqs::Irq::IRQ_ADC>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_EE_READY)
ISR(EE_READY_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_EE_READY>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_ANALOG_COMP)
ISR(ANALOG_COMP_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_ANALOG_COMP>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_TWI)
ISR(TWI_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_TWI>(IRQ_TASK_LIST());
}
#endif
#if defined(REGISTER_ALL_IRQS) || defined(USE_IRQ_SPM_READY)
ISR(SPM_READY_vect) {
  _irqs::execIrqTasks<0,  _irqs::Irq::IRQ_SPM_READY>(IRQ_TASK_LIST());
}
#endif
