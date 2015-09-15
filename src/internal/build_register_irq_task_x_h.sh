#!/bin/bash

for irq in IRQ_INT0 IRQ_INT1 IRQ_PCINT0 IRQ_PCINT1 IRQ_PCINT2 IRQ_WDT IRQ_TIMER2_COMPA IRQ_TIMER2_COMPB IRQ_TIMER2_OVF IRQ_TIMER1_CAPT IRQ_TIMER1_COMPA IRQ_TIMER1_COMPB IRQ_TIMER1_OVF IRQ_TIMER0_COMPA IRQ_TIMER0_COMPB IRQ_TIMER0_OVF IRQ_SPI_STC IRQ_USART_RX IRQ_USART_UDRE IRQ_USART_TX IRQ_ADC IRQ_EE_READY IRQ_ANALOG_COMP IRQ_TWI IRQ_SPM_READY 
do
  i=${irq/IRQ_/}
  file="register_irq_task_${i}.h"
  echo '#pragma push_macro("IRQ_NAME")' > $file
  echo "#define IRQ_NAME _irqs::$irq" >> $file
  echo '#include "register_irq_task.h"' >> $file
  echo "#define USE_$irq" >> $file
  echo '#pragma pop_macro("IRQ_NAME")' >> $file
done
