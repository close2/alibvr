#pragma once

#include "type_traits.h" // for is_same

#include <avr/io.h>
#include <avr/interrupt.h>
#include "irqs.h"
#include "macros.h"
#include "internal/timer.h"
#include "internal/clock_prescale.h"


/* BALI Notes
 * - maybe split timers into 2 templates def. for init and one for Tasks
 *   better: provide an alias just make sure we can not call ::init on alias
 *   < TaskA, Task, B, TaskOvF>
 *   using Timer0 = _timer0 <...>
 *                            ↑ invalid values
 * - maybe go through Irq Handler list and find out if there's a registered Task
 *   to know if IRQ should be enabled.
 * - explain modes (rename ctc?)
 * - move TimderDef (PIN_A, PIN_B) to ports.h?
 * - make || consistent (sometimes || is in front sometimes after)
 *   static_assert(X
 *              || Y ||
 *                 Z);
 * - add static_assert for Mode (In addition to ClockSelect)
 * - template <typename F, typename Timer>
 *   using MiniTaskA...
 */

namespace _timer0 {
  
  // this is only a possibility to prevent compilations of unsupported modes
  template <_timer::Mode Mode,
            _timer::ClockSelect ClockSelect>
  struct ModeSupport {};
  
  struct TimerDef {
    typedef uint8_t size_t;
    typedef PIN_6   PIN_A;
    typedef PIN_5   PIN_B;
    
    template <typename T>
    static void init(uint8_t& tcnt, uint8_t& timsk, uint8_t& tccra, uint8_t& tccrb, uint8_t& ocr_A, uint8_t& ocr_B, const uint8_t& initial_ocr_A, const uint8_t& initial_ocr_B) {
      const _timer::ClockSelect ClockSelect = T::UserDef::ClockSelect;
      const _timer::Mode Mode = T::UserDef::Mode;
       
      static_assert(ClockSelect == _timer::ClockSelect::Clock1
                 || ClockSelect == _timer::ClockSelect::Clock8
                 || ClockSelect == _timer::ClockSelect::Clock64
                 || ClockSelect == _timer::ClockSelect::Clock256
                 || ClockSelect == _timer::ClockSelect::Clock1024
                 || ClockSelect == _timer::ClockSelect::ClockTFalling
                 || ClockSelect == _timer::ClockSelect::ClockTRising
                 , "Invalid ClockSelect for Timer0");
      // clock select:
      switch (ClockSelect) {
        case _timer::ClockSelect::Clock1:
          tccrb |= _BV(CS00);
          break;
        case _timer::ClockSelect::Clock8:
          tccrb |= _BV(CS01);
          break;
        case _timer::ClockSelect::Clock64:
          tccrb |= _BV(CS01) | _BV(CS00);
          break;
        case _timer::ClockSelect::Clock256:
          tccrb |= _BV(CS02);
          break;
        case _timer::ClockSelect::Clock1024:
          tccrb |= _BV(CS02) | _BV(CS00);
          break;
        case _timer::ClockSelect::ClockTFalling:
          tccrb |= _BV(CS02) | _BV(CS01);
          break;
        case _timer::ClockSelect::ClockTRising:
          tccrb |= _BV(CS02) | _BV(CS01) | _BV(CS00);
          break;
      }
      
      if (Mode & 0b0001) tccra |= _BV(WGM00);
      if (Mode & 0b0100) tccra |= _BV(WGM01);
      if (Mode & 0b1000) tccrb |= _BV(WGM02);
    }
    
    static const auto IRQ_A =   _irqs::IRQ_TIMER0_COMPA;
    static const auto IRQ_B =   _irqs::IRQ_TIMER0_COMPB;
    static const auto IRQ_OVF = _irqs::IRQ_TIMER0_OVF;

    template <_timer::Unit U>
    static inline decltype(OCR0A)& ocr()  { return (U == _timer::Unit::A) ? OCR0A  : OCR0B; }
    
    static inline auto tcnt()  -> decltype(TCNT0)&  { return TCNT0; }
    static inline auto timsk() -> decltype(TIMSK0)& { return TIMSK0; }
    static inline auto tccra() -> decltype(TCCR0A)& { return TCCR0A; }
    static inline auto tccrb() -> decltype(TCCR0B)& { return TCCR0B; }
    
    template <_timer::Unit U>
    constexpr static inline uint8_t _com_0() { return (U == _timer::Unit::A) ? COM0A0 : COM0B0; }
    
    template <_timer::Unit U>
    constexpr static inline uint8_t _com_1() { return (U == _timer::Unit::A) ? COM0A1 : COM0B1; }
    
    template <_timer::Unit U>
    constexpr static inline uint8_t _ocie()  { return (U == _timer::Unit::A) ? OCIE0A : OCIE0B; }
    
    constexpr static inline uint8_t _toie()  { return TOIE0; }
  };
  
  template <_timer::Mode _Mode = _timer::Mode::FastPwm,
            _timer::ClockSelect _ClockSelect = _timer::ClockPrescale,
            // toggle operation in Pwm only possible for OutModeA and PhaseCorrectPwm
            _timer::OutputMode _OutModeA = _timer::OutputMode::NonInverting,
            _timer::OutputMode _OutModeB = _timer::OutputMode::Disconnected,
            uint8_t _SetDDR_A = 0,
            uint8_t _SetDDR_B = 0,
            typename _TaskA = _timer::DoNothing,
            typename _TaskB = _timer::DoNothing,
            typename _TaskOvF = _timer::DoNothing>
  using Timer = _timer::Timer<TimerDef, _timer::UserDef<_Mode, _ClockSelect, _OutModeA, _OutModeB, _SetDDR_A, _SetDDR_B, _TaskA, _TaskB, _TaskOvF>>;

}

template <_timer::ClockSelect ClockSelect = _timer::ClockPrescale,
          _timer::OutputMode OutModeA = _timer::OutputMode::Toggle,
          _timer::OutputMode OutModeB = _timer::OutputMode::Disconnected,
          uint8_t SetDDR_A = 0,
          uint8_t SetDDR_B = 0,
          typename TaskA = _timer::DoNothing,
          typename TaskB = _timer::DoNothing,
          typename TaskOvF = _timer::DoNothing>
using Timer0 = _timer0::Timer<_timer::Mode::Normal, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

template <_timer::ClockSelect ClockSelect = _timer::ClockPrescale,
          _timer::OutputMode OutModeA = _timer::OutputMode::Toggle,
          _timer::OutputMode OutModeB = _timer::OutputMode::Disconnected,
          uint8_t SetDDR_A = 0,
          uint8_t SetDDR_B = 0,
          typename TaskA = _timer::DoNothing,
          typename TaskB = _timer::DoNothing,
          typename TaskOvF = _timer::DoNothing>
using Ctc0 = _timer0::Timer<_timer::Mode::Ctc, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

template <_timer::ClockSelect ClockSelect = _timer::ClockPrescale,
          _timer::OutputMode OutModeA = _timer::OutputMode::NonInverting,
          _timer::OutputMode OutModeB = _timer::OutputMode::Disconnected,
          uint8_t SetDDR_A = 1,
          uint8_t SetDDR_B = 1,
          typename TaskA = _timer::DoNothing,
          typename TaskB = _timer::DoNothing,
          typename TaskOvF = _timer::DoNothing>
using FastPwm0 = _timer0::Timer<_timer::Mode::FastPwm, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

// either use #define NEW_PWM0 together with REGISTER_PWM0 or
#define REGISTER_PWM0 "internal/register_pwm0.h"
// use #define NEW_TIMER0 together with REGISTER_TIMER0
#define REGISTER_TIMER0 "internal/register_timer0.h"

#define _TIMER0_IN_USE _TIMER0_IN_USE

#ifdef _CLOCK_IN_USE
#  include "internal/timer0_is_clock.h"
#endif
