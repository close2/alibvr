#pragma once

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "bytes.h"
#include "irqs.h"
#include "internal/clock_prescale.h"

#ifndef ALIBVR_NAMESPACE_CLOCK
#  ifdef ALIBVR_NAMESPACE_PREFIX
#    define ALIBVR_NAMESPACE_CLOCK ALIBVR_NAMESPACE_PREFIX ## clock
#  else
#    define ALIBVR_NAMESPACE_CLOCK clock
#  endif
#endif

#pragma push_macro("F_CPU")
#ifndef F_CPU
#  include "internal/default_f_cpu.h"
#endif


/**
 * By default class related classes, enums,... are defined
 * inside the `clock` namespace.  If this creates a name clash with your
 * code you may modify the namespace name by setting
 * ALIBVR_NAMESPACE_CLOCK or ALIBVR_NAMESPACE_PREFIX.
 **/


/**
 * @brief This header file uses `timer0` to provide a system clock.
 * 
 * _Global irqs are enabled unless the macro: `CLOCK_NO_GLOBAL_IRQ` is
 * defined!_  Because `__attribute__((constructor))` is used including
 * this header file is enough to enable irqs globally!
 **/


namespace ALIBVR_NAMESPACE_CLOCK {
  static volatile bits64_t _clock;
  
  class _Clock {
  public:
    
    _Clock() {
      // we use timer0 (8bit)
      TCCR0B |= _BV(CS01) | _BV(CS00); // set prescaler to clkIO/64
      // if you change the prescaler here, don't forget to change internal/clock_prescale.h::NS::Prescale
      TCNT0 = 0;
      TIMSK0 |= _BV(TOIE0); // enable Timer-overflow interrupt
      
#     ifndef CLOCK_NO_GLOBAL_IRQ
        sei();
#     endif
    };
    
    operator uint8_t() {
      return TCNT0;
    }
    
    // The best documentation about inline asm in avr I've found:
    // http://www.nongnu.org/avr-libc/user-manual/inline_asm.html
    inline operator uint16_t() {
      bits16_t clock;
      asm volatile(
        "start_%=:\n\t"
        ""
        "lds %[c1], %[clock1]\n\t"
        "in %[c0], %[tcnt]\n\t"
        // Reread clock1 again.  If it has changed timer0 had an.
        // overflow.  Easily fixed by rereading everything again.
        "lds __tmp_reg__, %[clock1]\n\t"
        "sub __tmp_reg__, %[c1]\n\t"
        "brne start_%=\n\t"
        : [c0] "=r" (clock.avr.lo),
          [c1] "=r" (clock.avr.hi)
        : [tcnt] "I" (_SFR_IO_ADDR(TCNT0)),
          [clock1] "X" (&(_clock.avr.lo.avr.lo.avr.lo))
      );
      return clock.uint16;
    }
    
    inline operator uint32_t() {
      bits32_t clock;
      asm volatile(
        "start_%=:\n\t"
        ""
        "lds %[c1], %[clock1]\n\t"
        "lds %[c2], %[clock2]\n\t"
        "lds %[c3], %[clock3]\n\t"
        "in %[c0], %[tcnt]\n\t"
        // Reread clock0 again.  If it has changed timer0 had an.
        // overflow.  Easily fixed by rereading everything again.
        "lds __tmp_reg__, %[clock1]\n\t"
        "sub __tmp_reg__, %[c1]\n\t"
        "brne start_%=\n\t"
        : [c0] "=r" (clock.avr.lo.avr.lo),
          [c1] "=r" (clock.avr.lo.avr.hi),
          [c2] "=r" (clock.avr.hi.avr.lo),
          [c3] "=r" (clock.avr.hi.avr.hi)
        : [tcnt] "I" (_SFR_IO_ADDR(TCNT0)),
          [clock1] "X" (&(_clock.avr.lo.avr.lo.avr.lo)),
          [clock2] "X" (&(_clock.avr.lo.avr.lo.avr.hi)),
          [clock3] "X" (&(_clock.avr.lo.avr.hi.avr.lo))
      );
      return clock.uint32;
    }
    
    inline operator uint64_t() {
      bits64_t clock;
      asm volatile(
        "start_%=:\n\t"
        ""
        "lds %[c1], %[clock1]\n\t"
        "lds %[c2], %[clock2]\n\t"
        "lds %[c3], %[clock3]\n\t"
        "in %[c0], %[tcnt]\n\t"
        // Reread clock0 again.  If it has changed timer0 had an.
        // overflow.  Easily fixed by rereading everything again.
        "lds __tmp_reg__, %[clock1]\n\t"
        "sub __tmp_reg__, %[c1]\n\t"
        "brne start_%=\n\t"
        : [c0] "=r" (clock.avr.lo.avr.lo.avr.lo),
          [c1] "=r" (clock.avr.lo.avr.lo.avr.hi),
          [c2] "=r" (clock.avr.lo.avr.hi.avr.lo),
          [c3] "=r" (clock.avr.lo.avr.hi.avr.hi),
          [c4] "=r" (clock.avr.hi.avr.lo.avr.lo),
          [c5] "=r" (clock.avr.hi.avr.lo.avr.hi),
          [c6] "=r" (clock.avr.hi.avr.hi.avr.lo),
          [c7] "=r" (clock.avr.hi.avr.hi.avr.hi)
        : [tcnt] "I" (_SFR_IO_ADDR(TCNT0)),
          [clock1] "X" (&(_clock.avr.lo.avr.lo.avr.lo)),
          [clock2] "X" (&(_clock.avr.lo.avr.lo.avr.hi)),
          [clock3] "X" (&(_clock.avr.lo.avr.hi.avr.lo)),
          [clock4] "X" (&(_clock.avr.lo.avr.hi.avr.hi)),
          [clock5] "X" (&(_clock.avr.hi.avr.lo.avr.lo)),
          [clock6] "X" (&(_clock.avr.hi.avr.lo.avr.hi)),
          [clock7] "X" (&(_clock.avr.hi.avr.hi.avr.lo))
      );
      return clock.uint64;
    }
  };
  
  static _Clock Clock;
  
  void ClockIrqTask(_irqs::Irq I);
}

#define NEW_IRQ_TASK ALIBVR_NAMESPACE_CLOCK::ClockIrqTask
void NEW_IRQ_TASK(_irqs::Irq I) {
  uint8_t tmp;
  // for the common case the following code is a faster version of
  // Clock::_clock.uint64++;
  // The best documentation about inline asm in avr I've found:
  // http://www.nongnu.org/avr-libc/user-manual/inline_asm.html
  asm volatile(
    "in __tmp_reg__, __SREG__\n\t"
    "lds %[r], %[clock0]\n\t"
    "subi %[r], 0xFF\n\t"
    "sts %[clock0], %[r]\n\t"
    "brne endInc_%=\n\t"
    ""
    "lds %[r], %[clock1]\n\t"
    "subi %[r], 0xFF\n\t"
    "sts %[clock1], %[r]\n\t"
    "brne endInc_%=\n\t"
    ""
    "lds %[r], %[clock2]\n\t"
    "subi %[r], 0xFF\n\t"
    "sts %[clock2], %[r]\n\t"
    "brne endInc_%=\n\t"
    ""
    "lds %[r], %[clock3]\n\t"
    "subi %[r], 0xFF\n\t"
    "sts %[clock3], %[r]\n\t"
    "brne endInc_%=\n\t"
    ""
    "lds %[r], %[clock4]\n\t"
    "subi %[r], 0xFF\n\t"
    "sts %[clock4], %[r]\n\t"
    "brne endInc_%=\n\t"
    ""
    "lds %[r], %[clock5]\n\t"
    "subi %[r], 0xFF\n\t"
    "sts %[clock5], %[r]\n\t"
    "brne endInc_%=\n\t"
    ""
    "lds %[r], %[clock6]\n\t"
    "subi %[r], 0xFF\n\t"
    "sts %[clock6], %[r]\n\t"
    "brne endInc_%=\n\t"
    ""
    "lds %[r], %[clock7]\n\t"
    "subi %[r], 0xFF\n\t"
    "sts %[clock7], %[r]\n\t"
    ""
    "endInc_%=:\n\t"
    "out __SREG__, __tmp_reg__\n\t"
    : [r] "=d" (tmp)
    : [clock0] "X" (&(ALIBVR_NAMESPACE_CLOCK::_clock.avr.lo.avr.lo.avr.lo)),
      [clock1] "X" (&(ALIBVR_NAMESPACE_CLOCK::_clock.avr.lo.avr.lo.avr.hi)),
      [clock2] "X" (&(ALIBVR_NAMESPACE_CLOCK::_clock.avr.lo.avr.hi.avr.lo)),
      [clock3] "X" (&(ALIBVR_NAMESPACE_CLOCK::_clock.avr.lo.avr.hi.avr.hi)),
      [clock4] "X" (&(ALIBVR_NAMESPACE_CLOCK::_clock.avr.hi.avr.lo.avr.lo)),
      [clock5] "X" (&(ALIBVR_NAMESPACE_CLOCK::_clock.avr.hi.avr.lo.avr.hi)),
      [clock6] "X" (&(ALIBVR_NAMESPACE_CLOCK::_clock.avr.hi.avr.hi.avr.lo)),
      [clock7] "X" (&(ALIBVR_NAMESPACE_CLOCK::_clock.avr.hi.avr.hi.avr.hi))
   );
}
#include "internal/register_irq_task_TIMER0_OVF.h"
#undef NEW_IRQ_TASK

namespace ALIBVR_NAMESPACE_CLOCK {
  
  constexpr static uint32_t _cpu(uint32_t factor) {
    return F_CPU / factor;
  }
  
  template <uint32_t Factor, uint32_t P = _alibvr_clock_prescale::Prescale, typename M = uint16_t, typename T>
  constexpr static inline T time_to_units(const M& n) {
    /*
    const uint32_t cpu = F_CPU / Factor;
    if (cpu > 0) {
      if (cpu > P) {
        return n * (cpu / P);
      } else {
        return n / (P / cpu);
      }
    } else {
      static_assert(cpu > 0, "This division would cost, remove this assert if you are sure you want to do this");
      return n * (F_CPU / P) / Factor;
    }
    */
    static_assert(_cpu(Factor) > 0, "This division would cost, remove this assert if you are sure you want to do this");
    return (_cpu(Factor) > 0) 
      ? ((_cpu(Factor) > P)
        ? n * (_cpu(Factor) / P)
        : n / (P / _cpu(Factor)))
      : n * (F_CPU / P) / Factor;
  }
  
  template <uint32_t Factor, uint32_t P = _alibvr_clock_prescale::Prescale, typename T = uint16_t, typename M>
  constexpr static inline T units_to_time(const M& units) {
    /*
    const uint32_t cpu = F_CPU / Factor;
    if (cpu > 0) {
      if (cpu > P) {
        return units / (cpu / P);
      } else {
        uint32_t units_32b = units;
        return (units_32b * P) / cpu;
      }
    } else {
      static_assert(cpu > 0, "This division would cost, remove this assert if you are sure you want to do this");
      return units / (cpu / P);
    }
    */
    static_assert(_cpu(Factor) > 0, "This division would cost, remove this assert if you are sure you want to do this");
    return (_cpu(Factor) > 0) 
      ? ((_cpu(Factor) > P)
        ? units / (_cpu(Factor) / P)
        : (((uint32_t) units) * P) / _cpu(Factor))
      : units / (_cpu(Factor) / P);
  }
  
  // export clock select:
  typedef _alibvr_clock_select::ClockSelect ClockSelect;
  
  template <ClockSelect P = _alibvr_clock_prescale::Prescale, typename M = uint16_t, typename T = M>
  constexpr static inline T ms_to_units(const M& n) {
    return time_to_units<1000, (uint32_t) P, M, T>(n);
  }
  
  template <ClockSelect P = _alibvr_clock_prescale::Prescale, typename M = uint16_t, typename T = M>
  constexpr static inline T us_to_units(const M& n) {
    return time_to_units<1000000, (uint32_t) P, M, T>(n);
  }
  
  template <ClockSelect P = _alibvr_clock_prescale::Prescale, typename T = uint16_t, typename M = T>
  constexpr static inline M units_to_ms(const T& units) {
    return units_to_time<1000, (uint32_t) P, T, M>(units);
  }
  
  template <ClockSelect P = _alibvr_clock_prescale::Prescale, typename T = uint16_t, typename M = T>
  constexpr static inline M units_to_us(const T& units) {
    return units_to_time<1000000, (uint32_t) P, T, M>(units);
  }
  
  
  /*
   * There are 3 cases when we have reached the clock:
   * (1) --p--t--c-- (i.e. prev smaller than target, target smaller than clock)
   * current clock is higher than target.  As prev is smaller than target, we
   * passed the target.
   * 
   * (2) -t-c-----p-
   * Same analysis as prev. case.  Except that we have wrapped.
   * 
   * (3) -c-----p-t-
   * In this case only clock has wrapped.
   * 
   */
  template<typename T>
  static inline uint8_t clock_reached(const T& clock, const T& previous_clock, const T& target_clock) {
    uint8_t ret; // this variable makes debugging easier
    if (target_clock <= clock) {
      ret = previous_clock <= target_clock || clock < previous_clock; // (1) & (2)
    } else {
      ret = previous_clock <= target_clock && previous_clock > clock; // (3)
    }
    return ret;
  }
  
  template<typename T>
  static inline uint8_t clock_reached(const T& previous_clock, const T& target_clock) {
    return clock_reached((T) Clock, previous_clock, target_clock);
  }
  
}
#define _CLOCK_IN_USE _CLOCK_IN_USE

#ifdef _TIMER0_IN_USE
#  include "internal/timer0_is_clock.h"
#endif

#pragma pop_macro("F_CPU")
