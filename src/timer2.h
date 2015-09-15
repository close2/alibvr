#pragma once

#include "type_traits.h" // for is_same

#include <avr/io.h>
#include <avr/interrupt.h>
#include "irqs.h"
#include "macros.h"
#include "internal/timer.h"
#include "internal/clock_prescale.h"


namespace _timer2 {
  
  struct TimerDef {
    typedef uint8_t size_t;
    typedef PIN_11  PIN_A;
    typedef PIN_3   PIN_B;
    
    template <typename T>
    static void init(uint8_t& tcnt, uint8_t& timsk, uint8_t& tccra, uint8_t& tccrb, uint8_t& ocr_A, uint8_t& ocr_B, const uint8_t& initial_ocr_A, const uint8_t& initial_ocr_B) {
      const _timer::ClockSelect ClockSelect = T::UserDef::ClockSelect;
      const _timer::Mode Mode = T::UserDef::Mode;
       
      static_assert(ClockSelect == _timer::ClockSelect::Clock1
                 || ClockSelect == _timer::ClockSelect::Clock8
                 || ClockSelect == _timer::ClockSelect::Clock32
                 || ClockSelect == _timer::ClockSelect::Clock64
                 || ClockSelect == _timer::ClockSelect::Clock128
                 || ClockSelect == _timer::ClockSelect::Clock256
                 || ClockSelect == _timer::ClockSelect::Clock1024
                 , "Invalid ClockSelect for Timer2");
      // clock select:
      switch (ClockSelect) {
        case _timer::ClockSelect::Clock1:
          tccrb |= _BV(CS20);
          break;
        case _timer::ClockSelect::Clock8:
          tccrb |= _BV(CS21);
          break;
        case _timer::ClockSelect::Clock32:
          tccrb |= _BV(CS21) | _BV(CS20);
          break;
        case _timer::ClockSelect::Clock64:
          tccrb |= _BV(CS22);
          break;
        case _timer::ClockSelect::Clock128:
          tccrb |= _BV(CS22) | _BV(CS20);
          break;
        case _timer::ClockSelect::Clock256:
          tccrb |= _BV(CS22) | _BV(CS21);
          break;
        case _timer::ClockSelect::Clock1024:
          tccrb |= _BV(CS22) | _BV(CS21) | _BV(CS20);
          break;
      }
      
      if (Mode & 0b0001) tccra |= _BV(WGM20);
      if (Mode & 0b0100) tccra |= _BV(WGM21);
      if (Mode & 0b1000) tccrb |= _BV(WGM22);
    }
    
    static const auto IRQ_A =   _irqs::IRQ_TIMER2_COMPA;
    static const auto IRQ_B =   _irqs::IRQ_TIMER2_COMPB;
    static const auto IRQ_OVF = _irqs::IRQ_TIMER2_OVF;

    template <_timer::Unit U>
    static inline decltype(OCR2A)& ocr()  { return (U == _timer::Unit::A) ? OCR2A  : OCR2B; }
    
    static inline auto tcnt()  -> decltype(TCNT2)&  { return TCNT2; }
    static inline auto timsk() -> decltype(TIMSK2)& { return TIMSK2; }
    static inline auto tccra() -> decltype(TCCR2A)& { return TCCR2A; }
    static inline auto tccrb() -> decltype(TCCR2B)& { return TCCR2B; }
    
    template <_timer::Unit U>
    constexpr static inline uint8_t _com_0() { return (U == _timer::Unit::A) ? COM2A0 : COM2B0; }
    
    template <_timer::Unit U>
    constexpr static inline uint8_t _com_1() { return (U == _timer::Unit::A) ? COM2A1 : COM2B1; }
    
    template <_timer::Unit U>
    constexpr static inline uint8_t _ocie()  { return (U == _timer::Unit::A) ? OCIE2A : OCIE2B; }
    
    constexpr static inline uint8_t _toie()  { return TOIE2; }
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
using Timer2 = _timer2::Timer<_timer::Mode::Normal, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

template <_timer::ClockSelect ClockSelect = _timer::ClockPrescale,
          _timer::OutputMode OutModeA = _timer::OutputMode::Toggle,
          _timer::OutputMode OutModeB = _timer::OutputMode::Disconnected,
          uint8_t SetDDR_A = 0,
          uint8_t SetDDR_B = 0,
          typename TaskA = _timer::DoNothing,
          typename TaskB = _timer::DoNothing,
          typename TaskOvF = _timer::DoNothing>
using Ctc2 = _timer2::Timer<_timer::Mode::Ctc, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

template <_timer::ClockSelect ClockSelect = _timer::ClockPrescale,
          _timer::OutputMode OutModeA = _timer::OutputMode::NonInverting,
          _timer::OutputMode OutModeB = _timer::OutputMode::Disconnected,
          uint8_t SetDDR_A = 1,
          uint8_t SetDDR_B = 1,
          typename TaskA = _timer::DoNothing,
          typename TaskB = _timer::DoNothing,
          typename TaskOvF = _timer::DoNothing>
using FastPwm2 = _timer2::Timer<_timer::Mode::FastPwm, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

// either use #define NEW_PWM2 together with REGISTER_PWM2 or
#define REGISTER_PWM2 "internal/register_pwm2.h"
// use #define NEW_TIMER2 together with REGISTER_TIMER2
#define REGISTER_TIMER2 "internal/register_timer2.h"
