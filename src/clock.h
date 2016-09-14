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
 * @brief This file uses `Timer0` to provide a system clock.
 * 
 * The system clock is derived from the IO-clock and must be converted
 * to seconds.
 * 
 * Conversion functions are provided.  Try to stay with the template
 * versions, which can be calculated during compile time.  Conversion
 * of variables (with values unknown during compile time) will
 * probably include division code, which is both big and slow.
 * 
 * It is usually possible to compare the difference of 2 time instances
 * with a converted interval value:
 * 
 * Instead of (both examples in pseudocode):
 * 
 * ```C++
 * uint8_t clock1 = Clock;
 * // do something
 * uint8_t duration = Clock - clock1;
 * if (units_to_ms(duration) > 5) // work took more than 5 ms
 * 
 * ```
 * 
 * write
 * 
 * ```C++
 * uint8_t clock1 = Clock;
 * // do something
 * uint8_t duration = Clock - clock1;
 * if (duration > ms_to_units<5>()) // work took more than 5 ms
 * ```
 * 
 * _Global irqs are enabled unless the macro:
 * `ALIBVR_CLOCK_NO_GLOBAL_IRQ` or `ALIBVR_NO_GLOBAL_IRQ` is
 * defined!_  By including this header directly or indirectly global
 * irqs are enabled because of a static constructor in this file!
 * 
 * The clock is incremented inside the `Timer0` overflow irq handler.
 * To activate the registered irq handler add `#include REGISTER_IRQS`
 * after your `main` section.
 **/
namespace ALIBVR_NAMESPACE_CLOCK {
  
  /**
   * @brief This is the system clock.
   * 
   * This variable is changed inside the `Timer0` overflow irq handler.
   * `bits64_t` is a union type providing easy access to all bytes.
   * 
   * The prescaler is by default set to 1/64.
   * - At 8MHz _clock is incremented every  2'048ns.
   * - At 1MHz _clock is incremented every 16'384ns.
   * 
   * When retrieving the system clock the current `Timer0` counter is
   * added.  The precision of the system clock is therefore always
   * cpu clock / prescaler.
   **/
  static volatile bits64_t _clock;
  
  /**
   * @brief Initialization code and access to the _clock variable.
   * 
   * This class doesn't have any private members and its constructor is
   * only used once for a static variable.  The compiler will treat
   * the methods like static variables.
   * 
   * The cast converters allow us to use different optimized assembler
   * code for different types.
   * 
   * _You should never instantiate _Clock yourself!_
   **/
  class _Clock {
  public:
    
    /**
     * @brief Initializes `Timer0`.
     * 
     * Prescale is set to clkIO/64 and overflow irq is enabled.
     * 
     * Global irqs are then enabled unless `ALIBVR_CLOCK_NO_GLOBAL_IRQ`
     * or `ALIBVR_NO_GLOBAL_IRQ` is defined.
     * 
     * This constructor is only used once for the static `Clock`
     * variable.
     **/
    _Clock() {
      static_assert(_alibvr_clock_prescale::Prescale == _alibvr_clock_select::ClockSelect::Clock64,
                    "Clock assumes prescale is 1/64 but internal/clock_prescale is different.");
      // We use timer0 (8bit).
      TCCR0B |= _BV(CS01) | _BV(CS00); // set prescaler to clkIO/64
      // if you change the prescaler here, don't forget to change internal/clock_prescale.h::NS::Prescale
      TCNT0 = 0;
      TIMSK0 |= _BV(TOIE0); // enable Timer-overflow interrupt
      
#     if !defined ALIBVR_CLOCK_NO_GLOBAL_IRQ && !defined ALIBVR_NO_GLOBAL_IRQ
        sei();
#     endif
    };
    
    /**
     * @brief Converts the system clock to `uint8_t`.
     * 
     * Assuming a prescale of 1/64 `uint8_t` will overflow every
     * 2'048µs at 8MHz and every 16'384µs at 1MHz.
     * 
     * It is therefore not guaranteed that the returned value increases
     * when calling this function multiple times.
     * 
     * However.  If you stay below the mentioned overflow duration
     * substracting an earlier return value from a later return value
     * will give you a correct time difference (in "units").
     * 
     * This converter simply returns `TCNT0`.
     **/
    operator uint8_t() {
      return TCNT0;
    }
    
    /**
     * @brief Converts the system clock to `uint16_t`.
     * 
     * Assuming a prescale of 1/64 `uint16_t` will overflow every
     * 524'288µs at 8MHz and every 4'194'304µs at 1MHz.
     * 
     * It is therefore not guaranteed that the returned value increases
     * when calling this function multiple times.
     * 
     * However.  If you stay below the mentioned overflow duration
     * substracting an earlier return value from a later return value
     * will give you a correct time difference (in "units").
     * 
     * This converter returns the lowest byte of _clock << 8 && `TCNT0`.
     **/
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
    
    /**
     * @brief Converts the system clock to `uint32_t`.
     * 
     * Assuming a prescale of 1/64 `uint32_t` will overflow every
     * 9.5 hours at 8MHz and every 76 hours at 1MHz.
     * 
     * It is therefore not guaranteed that the returned value increases
     * when calling this function multiple times.
     * 
     * However.  If you stay below the mentioned overflow duration
     * substracting an earlier return value from a later return value
     * will give you a correct time difference (in "units").
     * 
     * This converter returns the lowest 3 bytes of _clock << 8 &&
     * `TCNT0`.
     **/
    // The best documentation about inline asm in avr I've found:
    // http://www.nongnu.org/avr-libc/user-manual/inline_asm.html
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
    
    /**
     * @brief Converts the system clock to `uint64_t`.
     * 
     * Assuming a prescale of 1/64 `uint64_t` will overflow every
     * 4'654'596 years at 8MHz and every 37'410'690 years at 1MHz.
     * 
     * This converter returns the lowest 7 bytes of _clock << 8 &&
     * `TCNT0`.
     **/
    // The best documentation about inline asm in avr I've found:
    // http://www.nongnu.org/avr-libc/user-manual/inline_asm.html
    inline operator uint64_t() {
      bits64_t clock;
      asm volatile(
        "start_%=:\n\t"
        ""
        "lds %[c1], %[clock1]\n\t"
        "lds %[c2], %[clock2]\n\t"
        "lds %[c3], %[clock3]\n\t"
        "lds %[c4], %[clock4]\n\t"
        "lds %[c5], %[clock5]\n\t"
        "lds %[c6], %[clock6]\n\t"
        "lds %[c7], %[clock7]\n\t"
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
/**
 * @brief This is the Timer0 Overflow handler, which increments the
 * system clock.
 * 
 * The compiler would generate less code, but execute more commands
 * on average than this assembler version.
 **/
void NEW_IRQ_TASK(_irqs::Irq __attribute__((unused)) I) {
  uint8_t tmp;
  // for the common case the following code is a faster version of
  // clock.uint64++;
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
  
  /**
   * @brief Convert "units" to real time units.
   * 
   * All inputs are template arguments.  This makes sure that the
   * compiler will be able to calculate everything during compile
   * time.
   * 
   * This is also possible with `constexpr` functions.  However as
   * a `constexpr` will work with dynamic values as well and not give
   * any warning I prefer the template version as it makes sure that
   * the inputs are known during compile time.
   * 
   * See units_to_ms and units_to_us for functions with simplified
   * input.
   * 
   * @tparam Units The system clock units, which should be converted.
   * @tparam Factor Use 1 second / Factor as return unit.  1'000 for ms
   *                1'000'000 for µs.
   * @tparam Prescale The prescale for `Timer0`.
   **/
  template <uint32_t Units,
            uint32_t Factor,
            uint32_t Prescale>
  uint32_t units_to_time() {
    // Promote all inputs.
    // This will be calculated during compile time.
    // By promoting, we avoid overflow problems.
    const uint64_t units = Units;
    const uint64_t factor = Factor;
    const uint64_t prescale = Prescale;
    
    const uint32_t cpu = F_CPU / factor;
    return (units * prescale) / cpu;
  }
  
  /**
   * @brief Convert real time units to "units".
   * 
   * See units_to_time for an explaination why inputs are template
   * arguments and why this isn't a constexpr function.
   * 
   * See ms_to_units and us_to_units for functions with simplified
   * input.
   * 
   * @tparam Time The time duration, which should be converted.
   * @tparam Factor Use 1 second / Factor as input unit.  1'000 for ms
   *                1'000'000 for µs.
   * @tparam Prescale The prescale for `Timer0`.
   **/
  template <uint32_t Time,
            uint32_t Factor,
            uint32_t Prescale>
  uint32_t time_to_units() {
    // Promote all inputs.
    // This will be calculated during compile time.
    // By promoting, we avoid overflow problems.
    const uint64_t time = Time;
    const uint64_t factor = Factor;
    const uint64_t prescale = Prescale;
    
    const uint32_t cpu = F_CPU / factor;
    return (time * cpu) / prescale;
  }


  // don't know why the depcreated attribute is ignored
  /**
   * @brief Convert "units" to real time units.
   * 
   * @deprecated Use template versions if possible.
   * 
   * See units_to_ms and units_to_us for functions with simplified
   * input.
   * 
   * @param units The system clock units, which should be converted.
   * @tparam Factor Use 1 second / Factor as return unit.  1'000 for ms
   *                1'000'000 for µs.
   * @tparam Prescale The prescale for `Timer0`.
   * @tparam T The type of units (automatically determined).
   **/
  template <uint32_t Factor,
            uint32_t Prescale = (uint32_t) _alibvr_clock_prescale::Prescale,
            typename T = uint16_t>
  __attribute__ ((deprecated("If possible use the template version.  "
    "This function probably bloats your code and might be very slow")))
  static inline uint32_t units_to_time(const T& units)
  {
    const uint32_t cpu = F_CPU / Factor;
    if (cpu > 0) {
      if (cpu > Prescale) {
        return units / (cpu / Prescale);
      } else {
        uint32_t units_32b = units;
        return (units_32b * Prescale) / cpu;
      }
    } else {
      return units / (cpu / Prescale);
    }
  }
  
  // don't know why the depcreated attribute is ignored
  /**
   * @brief Convert real time units to "units".
   * 
   * @deprecated Use template versions if possible.
   * 
   * See units_to_ms and units_to_us for functions with simplified
   * input.
   * 
   * @param n The time duration, which should be converted.
   * @tparam Factor Use 1 second / Factor as input unit.  1'000 for ms
   *                1'000'000 for µs.
   * @tparam Prescale The prescale for `Timer0`.
   * @tparam T The type of units (automatically determined).
   **/
  template <uint32_t Factor,
            uint32_t Prescale = (uint32_t) _alibvr_clock_prescale::Prescale,
            typename T = uint16_t>
  __attribute__ ((deprecated("If possible use the template version.  "
    "This function probably bloats your code and might be very slow")))
  static inline uint32_t time_to_units(const T& n) {
    const uint32_t cpu = F_CPU / Factor;
    if (cpu > 0) {
      if (cpu > Prescale) {
        return n * (cpu / Prescale);
      } else {
        return n / (Prescale / cpu);
      }
    } else {
      return n * (F_CPU / Prescale) / Factor;
    }
  }
  
  /**
   * @brief Export of `_alibvr_clock_select::ClockSelect`
   **/
  typedef _alibvr_clock_select::ClockSelect ClockSelect;
  
  
  // don't know why the depcreated attribute is ignored
  /**
   * @brief Convert ms to "units".
   * 
   * @deprecated Use template versions if possible.
   * 
   * @param n The time duration in ms, which should be converted.
   * @tparam P The prescale for `Timer0`.
   * @tparam T The type of units (automatically determined).
   **/
  template <ClockSelect P = _alibvr_clock_prescale::Prescale,
            typename T = uint16_t>
  __attribute__ ((deprecated("If possible use the template version.  "
    "This function probably bloats your code and might be very slow")))
  static inline T ms_to_units(const T& n) {
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    return time_to_units<1000, (uint32_t) P, T>(n);
#   pragma GCC diagnostic pop
  }
  
  
  // don't know why the depcreated attribute is ignored
  /**
   * @brief Convert µs to "units".
   * 
   * @deprecated Use template versions if possible.
   * 
   * @param n The time duration in µs, which should be converted.
   * @tparam P The prescale for `Timer0`.
   * @tparam T The type of units (automatically determined).
   **/
  template <ClockSelect P = _alibvr_clock_prescale::Prescale,
            typename T = uint16_t>
  __attribute__ ((deprecated("If possible use the template version.  "
    "This function probably bloats your code and might be very slow")))
  static inline uint32_t us_to_units(const T& n) {
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    return time_to_units<1000000, (uint32_t) P, T>(n);
#   pragma GCC diagnostic pop
  }
  
  // don't know why the depcreated attribute is ignored
  /**
   * @brief Convert "units" to ms.
   * 
   * @deprecated Use template versions if possible.
   * 
   * @param units The time duration in units, which should be converted.
   * @tparam P The prescale for `Timer0`.
   * @tparam T The type of units (automatically determined).
   **/
  template <ClockSelect P = _alibvr_clock_prescale::Prescale,
            typename T = uint16_t>
  __attribute__ ((deprecated("If possible use the template version.  "
    "This function probably bloats your code and might be very slow")))
  static inline uint32_t units_to_ms(const T& units) {
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    return units_to_time<1000, (uint32_t) P, T>(units);
#   pragma GCC diagnostic pop
  }
  
  // don't know why the depcreated attribute is ignored
  /**
   * @brief Convert "units" to µs.
   * 
   * @deprecated Use template versions if possible.
   * 
   * @param units The time duration in units, which should be converted.
   * @tparam P The prescale for `Timer0`.
   * @tparam T The type of units (automatically determined).
   **/
  template <ClockSelect P = _alibvr_clock_prescale::Prescale,
            typename T = uint16_t>
  __attribute__ ((deprecated("If possible use the template version.  "
    "This function probably bloats your code and might be very slow")))
  static inline uint32_t units_to_us(const T& units) {
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    return units_to_time<1000000, (uint32_t) P, T>(units);
#   pragma GCC diagnostic pop
  }
  
  /**
   * @brief Convert ms to "units".
   * 
   * See units_to_time for an explaination why inputs are template
   * arguments and why this isn't a constexpr function.
   * 
   * 
   * @tparam Time The time duration in ms, which should be converted.
   * @tparam P The prescale for `Timer0`.
   **/
  template <uint32_t Time,
            ClockSelect P = _alibvr_clock_prescale::Prescale>
  static inline uint32_t ms_to_units() {
    return time_to_units<Time, 1000, (uint32_t) P>();
  }
  
  /**
   * @brief Convert µs to "units".
   * 
   * See units_to_time for an explaination why inputs are template
   * arguments and why this isn't a constexpr function.
   * 
   * @tparam Time The time duration in µ, which should be converted.
   * @tparam P The prescale for `Timer0`.
   **/
  template <uint32_t Time,
            ClockSelect P = _alibvr_clock_prescale::Prescale>
  static inline uint32_t us_to_units() {
    return time_to_units<Time, 1000000, (uint32_t) P>();
  }
  
  /**
   * @brief Convert "units" to ms.
   * 
   * See units_to_time for an explaination why inputs are template
   * arguments and why this isn't a constexpr function.
   * 
   * @tparam Units The units which should be converted.
   * @tparam P The prescale for `Timer0`.
   **/
  template <uint32_t Units,
            ClockSelect P = _alibvr_clock_prescale::Prescale>
  static inline uint32_t units_to_ms() {
    return units_to_time<Units, 1000, (uint32_t) P>();
  }
  
  /**
   * @brief Convert "units" to µs.
   * 
   * See units_to_time for an explaination why inputs are template
   * arguments and why this isn't a constexpr function.
   * 
   * @tparam Units The units which should be converted.
   * @tparam P The prescale for `Timer0`.
   **/
  template <uint32_t Units,
            ClockSelect P = _alibvr_clock_prescale::Prescale>
  static inline uint32_t units_to_us() {
    return units_to_time<Units, 1000000, (uint32_t) P>();
  }
  
  
  /**
   * @brief Calculate if the difference between previous_clock and
   *   current_clock is bigger than duration.
   * 
   * This function will return a correct result, even if Clock has
   * wrapped around.  It is however up to the caller to make sure that
   * the clock type T is big enough.
   * 
   * Unless you have multiple comparisons with different durations,
   * use the simpler duration_passed function, which only requires
   * previous_clock.
   * 
   * @param previous_clock The clock value (in "units") to which
   *   duration is added.  It is ok if previous_clock equals
   *   current_clock.
   * @param current_clock The clock value (in "units") which should be
   *   farther in the future than previous_clock + duration for this
   *   function to return true.
   * @param duration The duration in "units" which should pass since
   *   previous_clock.  See ms_to_units and us_to_units.
   **/
  template<typename T, typename U>
  static inline uint8_t duration_passed(const T& previous_clock, const T& current_clock, const U& duration) {
    return (current_clock - previous_clock) > duration;
  }
  
  /**
   * @brief Calculate if duration has passed since previous_clock.
   * 
   * This function will return a correct result, even if Clock has
   * wrapped around.  It is however up to the caller to make sure that
   * the clock type T is big enough.
   * 
   * @param previous_clock The clock value (in "units") to which
   *   duration is added.  It is ok if previous_clock is the current
   *   Clock: `uint16_t prev = Clock; while (!duration_passed(prev, duration)) prev = Clock;`
   * @param duration The duration in "units" which should pass since
   *   previous_clock.  See ms_to_units and us_to_units.
   **/
  template<typename T, typename U>
  static inline uint8_t duration_passed(const T& previous_clock, const U& duration) {
    return duration_passed(previous_clock, (T) Clock, duration);
  }
}
#define _CLOCK_IN_USE _CLOCK_IN_USE

#ifdef _TIMER0_IN_USE
#  include "internal/timer0_is_clock.h"
#endif

#pragma pop_macro("F_CPU")
