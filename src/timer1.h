#pragma once

#include "type_traits.h" // for is_same

#include <avr/io.h>
#include <avr/interrupt.h>
#include "irqs.h"
#include "macros.h"
#include "internal/timer.h"

namespace _timer1 {
  
  struct TimerDef {
    typedef uint16_t size_t;
    typedef PIN_9    PIN_A;
    typedef PIN_10   PIN_B;
    
    template <typename T>
    static void init(uint8_t& tcnt, uint8_t& timsk, uint8_t& tccra, uint8_t& tccrb, uint16_t& ocr_A, uint16_t& ocr_B, const uint16_t& initial_ocr_A, const uint16_t& initial_ocr_B) {
      const _timer::ClockSelect ClockSelect = T::UserDef::ClockSelect;
      const _timer::Mode Mode = T::UserDef::Mode;
       
      static_assert(ClockSelect == _timer::ClockSelect::Clock1 ||
                    ClockSelect == _timer::ClockSelect::Clock8 ||
                    ClockSelect == _timer::ClockSelect::Clock64 ||
                    ClockSelect == _timer::ClockSelect::Clock256 ||
                    ClockSelect == _timer::ClockSelect::Clock1024 ||
                    ClockSelect == _timer::ClockSelect::ClockTFalling ||
                    ClockSelect == _timer::ClockSelect::ClockTRising,
                   "Invalid ClockSelect for Timer1");
      // clock select:
      switch (ClockSelect) {
        case _timer::ClockSelect::Clock1:
          tccrb |= _BV(CS10);
          break;
        case _timer::ClockSelect::Clock8:
          tccrb |= _BV(CS11);
          break;
        case _timer::ClockSelect::Clock64:
          tccrb |= _BV(CS11) | _BV(CS10);
          break;
        case _timer::ClockSelect::Clock256:
          tccrb |= _BV(CS12);
          break;
        case _timer::ClockSelect::Clock1024:
          tccrb |= _BV(CS12) | _BV(CS10);
          break;
        case _timer::ClockSelect::ClockTFalling:
          tccrb |= _BV(CS12) | _BV(CS11);
          break;
        case _timer::ClockSelect::ClockTRising:
          tccrb |= _BV(CS12) | _BV(CS11) | _BV(CS10);
          break;
        default:
          break; // will never happen because of static_assert
      }

      if ((uint8_t) Mode & 0b0001) tccra |= _BV(WGM10);
      if ((uint8_t) Mode & 0b0010) tccra |= _BV(WGM11);
      if ((uint8_t) Mode & 0b0100) tccrb |= _BV(WGM12);
      if ((uint8_t) Mode & 0b1000) tccrb |= _BV(WGM13);
    }
    
    static const auto IRQ_A =   _irqs::IRQ_TIMER1_COMPA;
    static const auto IRQ_B =   _irqs::IRQ_TIMER1_COMPB;
    static const auto IRQ_OVF = _irqs::IRQ_TIMER1_OVF;

    template <_timer::Unit U>
    static inline decltype(OCR1A)& ocr()  { return (U == _timer::Unit::A) ? OCR1A  : OCR1B; }
    
    static inline auto tcnt()  -> decltype(TCNT1)&  { return TCNT1; }
    static inline auto timsk() -> decltype(TIMSK1)& { return TIMSK1; }
    static inline auto tccra() -> decltype(TCCR1A)& { return TCCR1A; }
    static inline auto tccrb() -> decltype(TCCR1B)& { return TCCR1B; }
    
    template <_timer::Unit U>
    constexpr static inline uint8_t _com_0() { return (U == _timer::Unit::A) ? COM1A0 : COM1B0; }
    
    template <_timer::Unit U>
    constexpr static inline uint8_t _com_1() { return (U == _timer::Unit::A) ? COM1A1 : COM1B1; }
    
    template <_timer::Unit U>
    constexpr static inline uint8_t _ocie()  { return (U == _timer::Unit::A) ? OCIE1A : OCIE1B; }
    
    constexpr static inline uint8_t _toie()  { return TOIE1; }
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
using Timer1 = _timer1::Timer<_timer::Mode::Normal, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

template <_timer::ClockSelect ClockSelect = _timer::ClockPrescale,
          _timer::OutputMode OutModeA = _timer::OutputMode::Toggle,
          _timer::OutputMode OutModeB = _timer::OutputMode::Disconnected,
          uint8_t SetDDR_A = 0,
          uint8_t SetDDR_B = 0,
          typename TaskA = _timer::DoNothing,
          typename TaskB = _timer::DoNothing,
          typename TaskOvF = _timer::DoNothing>
using Ctc1 = _timer1::Timer<_timer::Mode::Ctc, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

template <_timer::ClockSelect ClockSelect = _timer::ClockPrescale,
          _timer::OutputMode OutModeA = _timer::OutputMode::NonInverting,
          _timer::OutputMode OutModeB = _timer::OutputMode::Disconnected,
          uint8_t SetDDR_A = 1,
          uint8_t SetDDR_B = 1,
          typename TaskA = _timer::DoNothing,
          typename TaskB = _timer::DoNothing,
          typename TaskOvF = _timer::DoNothing>
using FastPwm1 = _timer1::Timer<_timer::Mode::FastPwm, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

template <_timer::ClockSelect ClockSelect = _timer::ClockPrescale,
          _timer::OutputMode OutModeA = _timer::OutputMode::NonInverting,
          _timer::OutputMode OutModeB = _timer::OutputMode::Disconnected,
          uint8_t SetDDR_A = 1,
          uint8_t SetDDR_B = 1,
          typename TaskA = _timer::DoNothing,
          typename TaskB = _timer::DoNothing,
          typename TaskOvF = _timer::DoNothing>
using PhaseAndFrequencyCorrectPwm1 = _timer1::Timer<_timer::Mode::PhaseAndFrequencyCorrectPwm, ClockSelect, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB, TaskOvF>;

// either use #define NEW_PWM1 together with REGISTER_PWM1 or
#define REGISTER_PWM1 "internal/register_pwm1.h"
// use #define NEW_TIMER1 together with REGISTER_TIMER1
#define REGISTER_TIMER1 "internal/register_timer1.h"
