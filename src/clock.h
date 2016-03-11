#pragma once

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "bytes.h"
#include "irqs.h"
#include "internal/clock_prescale.h"

/* BALI notes
 * replase get_clock with asm:
 * start_%=:
 * lds %[ch1], %[r]
 * lds %[cl], %[tcnt]
 * lds %[ch2] %[r]
 * sub %[ch2], %[ch1]
 * brne start_%=
 */
namespace _clock {
  static volatile bits64_t clock;
};

#define NEW_IRQ_TASK ClockIrqTask
IRQ_TASK(NEW_IRQ_TASK) {
  uint8_t tmp;
  // for the common case the following code is a faster version of
  // _clock::clock.uint64++;
  asm volatile(
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
    : [r] "=d" (tmp)
    : [clock0] "X" (&(_clock::clock.avr.lo.avr.lo.avr.lo)),
      [clock1] "X" (&(_clock::clock.avr.lo.avr.lo.avr.hi)),
      [clock2] "X" (&(_clock::clock.avr.lo.avr.hi.avr.lo)),
      [clock3] "X" (&(_clock::clock.avr.lo.avr.hi.avr.hi)),
      [clock4] "X" (&(_clock::clock.avr.hi.avr.lo.avr.lo)),
      [clock5] "X" (&(_clock::clock.avr.hi.avr.lo.avr.hi)),
      [clock6] "X" (&(_clock::clock.avr.hi.avr.hi.avr.lo)),
      [clock7] "X" (&(_clock::clock.avr.hi.avr.hi.avr.hi))
   );
}
#include "internal/register_irq_task_TIMER0_OVF.h"
#undef NEW_IRQ_TASK

namespace _clock {
  constexpr static uint32_t _cpu(uint32_t factor) {
    return F_CPU / factor;
  }
  
  template <uint32_t Factor, uint32_t Prescale = _clock::Prescale, typename M = uint16_t, typename T>
  constexpr static inline T time_to_units(const M& n) {
    /*
    const uint32_t cpu = F_CPU / Factor;
    if (cpu > 0) {
      if (cpu > Prescale) {
        return n * (cpu / Prescale);
      } else {
        return n / (Prescale / cpu);
      }
    } else {
      static_assert(cpu > 0, "This division would cost, remove this assert if you are sure you want to do this");
      return n * (F_CPU / Prescale) / Factor;
    }
    */
    static_assert(_cpu(Factor) > 0, "This division would cost, remove this assert if you are sure you want to do this");
    return (_cpu(Factor) > 0) 
      ? ((_cpu(Factor) > Prescale)
        ? n * (_cpu(Factor) / Prescale)
        : n / (Prescale / _cpu(Factor)))
      : n * (F_CPU / Prescale) / Factor;
  }
  
  template <uint32_t Factor, uint32_t Prescale = _clock::Prescale, typename T = uint16_t, typename M>
  constexpr static inline T units_to_time(const M& units) {
    /*
    const uint32_t cpu = F_CPU / Factor;
    if (cpu > 0) {
      if (cpu > Prescale) {
        return units / (cpu / Prescale);
      } else {
        uint32_t units_32b = units;
        return (units_32b * Prescale) / cpu;
      }
    } else {
      static_assert(cpu > 0, "This division would cost, remove this assert if you are sure you want to do this");
      return units / (cpu / Prescale);
    }
    */
    static_assert(_cpu(Factor) > 0, "This division would cost, remove this assert if you are sure you want to do this");
    return (_cpu(Factor) > 0) 
      ? ((_cpu(Factor) > Prescale)
        ? units / (_cpu(Factor) / Prescale)
        : (((uint32_t) units) * Prescale) / _cpu(Factor))
      : units / (_cpu(Factor) / Prescale);
  }
}

template <_clock_select::ClockSelect Prescale = _clock::Prescale, typename M = uint16_t, typename T = M>
constexpr static inline T ms_to_units(const M& n) {
  return _clock::time_to_units<1000, (uint32_t) Prescale, M, T>(n);
}

template <_clock_select::ClockSelect Prescale = _clock::Prescale, typename M = uint16_t, typename T = M>
constexpr static inline T us_to_units(const M& n) {
  return _clock::time_to_units<1000000, (uint32_t) Prescale, M, T>(n);
}

template <_clock_select::ClockSelect Prescale = _clock::Prescale, typename T = uint16_t, typename M = T>
constexpr static inline M units_to_us(const T& units) {
  return _clock::units_to_time<1000000, (uint32_t) Prescale, T, M>(units);
}

template <_clock_select::ClockSelect Prescale = _clock::Prescale, typename T = uint16_t, typename M = T>
constexpr static inline M units_to_ms(const T& units) {
  return _clock::units_to_time<1000, (uint32_t) Prescale, T, M>(units);
}

template<typename T>
static T get_clock() {
  return T::unimplemented();
}

template<>
uint8_t get_clock<uint8_t>() {
  return TCNT0;
}

// TODO replace this with inline assembler
template<>
uint16_t get_clock<uint16_t>() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    const uint8_t tcnt = TCNT0;
    // use tcnt to make it harder for the compiler to optimize this
    // incorrectly
    if (TIFR0 & _BV(TOV0) && tcnt != 0xFF) {
      // irq pending
      return _clock::clock.avr.lo.avr.lo.uint16 << 8 | 0xFF;
    } else return _clock::clock.avr.lo.avr.lo.uint16 << 8 | tcnt;
  }
  return 0;
}

template<>
uint32_t get_clock<uint32_t>() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    const uint8_t tcnt = TCNT0;
    // use tcnt to make it harder for the compiler to optimize this
    // incorrectly
    if (TIFR0 & _BV(TOV0) && tcnt != 0xFF) {
      // irq pending
      return _clock::clock.avr.lo.uint32 << 8 | 0xFF;
    } else return _clock::clock.avr.lo.uint32 << 8 | tcnt;
  }
  return 0;
}

template<>
uint64_t get_clock<uint64_t>() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    const uint8_t tcnt = TCNT0;
    // use tcnt to make it harder for the compiler to optimize this
    // incorrectly
    if (TIFR0 & _BV(TOV0) && tcnt != 0xFF) {
      // irq pending
      return _clock::clock.uint64 << 8 | 0xFF;
    } else return _clock::clock.uint64 << 8 | tcnt;
  }
  return 0;
}

__attribute__((constructor))
static void init_timer0() {
  // we use timer0 (8bit)
  TCCR0B |= _BV(CS01) | _BV(CS00); // set prescaler to clkIO/64
  // if you change the prescaler here, don't forget to change internal/clock_prescale.h::_clock::Prescale
  TCNT0 = 0;
  TIMSK0 |= _BV(TOIE0); // enable Timer-overflow interrupt
  
  sei();
};

/*
 * There are 3 cases when we have reched the clock:
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
  return clock_reached(get_clock<T>(), previous_clock, target_clock);
}

#define _CLOCK_IN_USE _CLOCK_IN_USE

#ifdef _TIMER0_IN_USE
#  include "internal/timer0_is_clock.h"
#endif
