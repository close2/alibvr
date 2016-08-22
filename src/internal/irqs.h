#pragma once

// mostly copied from: http://stackoverflow.com/questions/18701798/building-and-accessing-a-list-of-types-at-compile-time/18704609#18704609

#include "task_list.h"

#define IRQ_TASK_LIST _task_list::TaskListEmpty

namespace _irqs {
  
  enum Irq {
    //IRQ_RESET = 0x000,
    IRQ_INT0 = 0x001,
    IRQ_INT1 = 0x002,
    IRQ_PCINT0 = 0x003,
    IRQ_PCINT1 = 0x004,
    IRQ_PCINT2 = 0x005,
    IRQ_WDT = 0x006,
    IRQ_TIMER2_COMPA = 0x007,
    IRQ_TIMER2_COMPB = 0x008,
    IRQ_TIMER2_OVF = 0x009,
    IRQ_TIMER1_CAPT = 0x00A,
    IRQ_TIMER1_COMPA = 0x00B,
    IRQ_TIMER1_COMPB = 0x00C,
    IRQ_TIMER1_OVF = 0x00D,
    IRQ_TIMER0_COMPA = 0x00E,
    IRQ_TIMER0_COMPB = 0x00F,
    IRQ_TIMER0_OVF = 0x010,
    IRQ_SPI_STC = 0x011,
    IRQ_USART_RX = 0x012,
    IRQ_USART_UDRE = 0x013,
    IRQ_USART_TX = 0x014,
    IRQ_ADC = 0x015,
    IRQ_EE_READY = 0x016,
    IRQ_ANALOG_COMP = 0x017,
    IRQ_TWI = 0x018,
    IRQ_SPM_READY = 0x019
  };
  

  typedef void (*handler_f)(enum Irq);
  template <handler_f F, enum Irq I>
  struct IrqTaskWrapper {
    static const enum Irq irq = I;
    
    static inline void handle(enum Irq i) {
      F(i);
    }
  };

  
  template <uint8_t B>
  struct State {
    static const uint8_t state = B;
  };
  
  
  // Whenever an IRQ is received all Irq-tasks are considered (see execIrqTasks).
  // If at least one Taks is registered for this IRQ (T::I::irq) Handled changes
  // to State<1>.
  // Otherwise the specialisation for no Task left will call the resetVector.
  // (I.e. reset the avr)
  // FIXME move this to internal, so that register_irqs can include it.
  //template <typename Handled, typename I, template <typename ...> class List, typename T, typename ...Tasks>
  //static void execIrqTasks();
  
  template <typename Handled, Irq I, typename T>
  inline static void execIrqTasks(_task_list::TaskList<T>) {
    if (T::irq == I) {
      T::handle(I);
      return;
    }
    if (!Handled::state) {
      // call the resetVector
      ((void (*)(void))0x00)();
      for (;;) {}
    }
  }

  
  template <typename Handled, Irq I, typename T, typename ...Tasks>
  inline static void execIrqTasks(_task_list::TaskList<T, Tasks...>) {
    if (T::irq == I) {
      T::handle(I);
      execIrqTasks<State<1>, I>(_task_list::TaskList<Tasks...>());
    } else {
      execIrqTasks<Handled, I>(_task_list::TaskList<Tasks...>());
    }
  }
}