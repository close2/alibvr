#pragma once

#include <avr/io.h>
#include "irqs.h"
#include "macros.h"

namespace _timer {
  
  enum Unit {
    A,
    B
  };

  struct DoNothing {
    static uint8_t is_enabled() { return false; }
    static uint8_t run(const uint8_t& clock) { return 0; }
    static void run() {};
  };

  /*
  enum Mode {
    MiniTask,
    Ctc,
    Pwm
  };
  */
  
  enum PwmMode {
    FastPwm,
    PhaseCorrectPwm,
    PhaseAndFrequencyCorrectPwm
  };
  
  enum OutputMode {
    Disconnected,
    Inverting,
    NonInverting
  };
}

template <class TimerN,
          _timer::PwmMode Mode = _timer::FastPwm,
          _timer::OutputMode OutModeA = _timer::NonInverting,
          _timer::OutputMode OutModeB = _timer::Disconnected,
          uint8_t SetDDR_A = 1,
          uint8_t SetDDR_B = 1,
          typename TaskA = _timer::DoNothing,
          typename TaskB = _timer::DoNothing,
          typename TaskOvF = _timer::DoNothing>
class Pwm {
public:
  typedef typename TimerN::size_t size_t;

private:
  template <_timer::Unit U, uint8_t SetDDR, typename PIN, uint8_t ForceSetOcr, typename R>
  static inline void _init(R& tccra,
                           R& tccrb,
                           R& tcnt,
                           size_t& ocr,
                           R& timsk,
                           const size_t& initial_pwm,
                           const _timer::OutputMode& mode) {
    if (mode == _timer::Disconnected && !ForceSetOcr) return;
    
    ocr = initial_pwm;
    
    const uint8_t _com_1 = TimerN::template _com_1<U>();
    const uint8_t _com_0 = TimerN::template _com_0<U>();
    
    switch (mode) {
    case _timer::Disconnected:
      // set them both to 0
      tccra &= ~(_BV(_com_1) | _BV(_com_0));
      break;
    case _timer::Inverting:
      if (SetDDR) SET_BIT(PIN, DDR, 1);
      tccra |= _BV(_com_1) | _BV(_com_0);
      break;
    case _timer::NonInverting:
      if (SetDDR) SET_BIT(PIN, DDR, 1);
      tccra |= _BV(_com_1);
      tccra &= ~(_BV(_com_0));
      break;
    }
  }

  template <_timer::Unit U, uint8_t SetDDR, typename PIN, typename R>
  static inline void _init(R& tccra,
                           R& tccrb,
                           R& tcnt,
                           size_t& ocr,
                           R& timsk,
                           const size_t& initial_pwm,
                           const _timer::OutputMode& mode,
                           _timer::DoNothing* const type) {
    _init<U, SetDDR, PIN, 0>(tccra, tccrb, tcnt, ocr, timsk, initial_pwm, mode);
  }

  template <_timer::Unit U, uint8_t SetDDR, typename PIN, typename T, typename R>
  static inline void _init(R& tccra,
                           R& tccrb,
                           R& tcnt,
                           size_t& ocr,
                           R& timsk,
                           const size_t& initial_pwm,
                           const _timer::OutputMode& mode,
                           T* const type) {
    _init<U, SetDDR, PIN, 1>(tccra, tccrb, tcnt, ocr, timsk, initial_pwm, mode);
    // activate output compare match interrupt (task is not DoNothing
    timsk |= _BV(TimerN::ocie());
  }

public:

  template <typename I>
  static inline void handle(I) {
    if (I::irq == TimerN::IRQ_A) {
      if (TaskA::is_enabled()) {
        const size_t prev_pwm = get_pwm_A();
        const size_t next_pwm = TaskA::run(prev_pwm);
        if (prev_pwm != next_pwm) set_pwm_A(next_pwm);
      }
    }
    if (I::irq == TimerN::IRQ_B) {
      if (TaskB::is_enabled()) {
        const size_t prev_pwm = get_pwm_B();
        const size_t next_pwm = TaskB::run(prev_pwm);
        if (prev_pwm != next_pwm) set_pwm_B(next_pwm);
      }
    }
    if (I::irq == TimerN::IRQ_OVF) {
      if (TaskOvF::is_enabled()) {
        TaskOvF::handle(I());
      }
    }
  }
  
  static inline void init(const size_t& initial_pwm_A,
                          const size_t& initial_pwm_B) {
    uint8_t tcnt = 0;
    uint8_t timsk = 0;
    uint8_t tccra = 0;
    uint8_t tccrb = 0;
    size_t ocr_A = 0;
    size_t ocr_B = 0;
    
    _init<_timer::A, SetDDR_A, typename TimerN::PIN_A>(tccra, tccrb, tcnt, ocr_A, timsk, initial_pwm_A, OutModeA, (TaskA*) nullptr);
    _init<_timer::B, SetDDR_B, typename TimerN::PIN_B>(tccra, tccrb, tcnt, ocr_B, timsk, initial_pwm_B, OutModeB, (TaskB*) nullptr);
    
    typedef TimerN _T;
    
    if (TimerN::timer_counter == 0) {
      // no clock select because clock.h does this for us
      // we also don't support ocrA as Top value
      // → not necessary to change tccr0b 
      // fast pwm
      tccra |= _BV(_T::_wgm_0()) | _BV(_T::_wgm_1());
      // don't set tcnt for timer0 (clock.h wouldn't like this)
    }
    
    // now assign those calculated values to the real registers
    if (tcnt)  _T::tcnt()  = tcnt;
    if (timsk) _T::timsk() = timsk; 
    if (tccra) _T::tccra() = tccra;
    if (tccrb) _T::tccrb() = tccrb;
    //if (ocr_A) _T::ocr<_timer::A>() = ocr_A;
    //if (ocr_B) _T::ocr<_timer::B>() = ocr_B;
    const _timer::Unit A = _timer::A;
    if (ocr_A) TimerN::template ocr<A>() = ocr_A;
    const _timer::Unit B = _timer::B;
    if (ocr_B) TimerN::template ocr<B>() = ocr_B;
  }
  
  template <_timer::Unit U>
  static inline void set_pwm(const size_t& new_pwm) {
    TimerN::template ocr<U>() = new_pwm;
  }
  
  static inline void set_pwm_A(const size_t& new_pwm) {
    set_pwm<_timer::A>(new_pwm);
  }
  
  static inline void set_pwm_B(const size_t& new_pwm) {
    set_pwm<_timer::B>(new_pwm);
  }
  
  template <_timer::Unit U>
  static inline size_t get_pwm() {
    return TimerN::template ocr<U>();
  }
  
  static inline size_t get_pwm_A() {
    return get_pwm<_timer::A>();
  }
  
  static inline size_t get_pwm_B() {
    return get_pwm<_timer::B>();
  }
};

#define REGISTER_PWM0 "internal/register_pwm0.h"


// definitions for different Timers
namespace _timer {

  struct Timer0 {
    typedef uint8_t size_t;

    static const _irqs::Irq IRQ_A = _irqs::IRQ_TIMER0_COMPA;
    static const _irqs::Irq IRQ_B = _irqs::IRQ_TIMER0_COMPB;
    static const _irqs::Irq IRQ_OVF = _irqs::IRQ_TIMER0_OVF;
    
    typedef PIN_D6 PIN_A;
    typedef PIN_D5 PIN_B;
    
    static const uint8_t timer_counter = 0;

    template <enum Unit U>
    static inline decltype(OCR0A)& ocr() {
      return (U == _timer::A) ? OCR0A : OCR0B;
    }

    static inline decltype(TIMSK0)& timsk() {
      return TIMSK0;
    }
    
    static inline decltype(TCNT0)& tcnt() {
      return TCNT0;
    }
    
    static inline decltype(TCCR0A)& tccra() {
      return TCCR0A;
    }
    
    static inline decltype(TCCR0B)& tccrb() {
      return TCCR0B;
    }
    
    // bit locations (start with _)
    template <enum Unit U>
    static inline uint8_t _com_0() {
      return (U == _timer::A) ? COM0A0 : COM0B0;
    }
    
    template <enum Unit U>
    static inline uint8_t _com_1() {
      return (U == _timer::A) ? COM0A1 : COM0B1;
    }
    
    static inline uint8_t _wgm_0() {
      return WGM00;
    }
    
    static inline uint8_t _wgm_1() {
      return WGM01;
    }
    
    static inline uint8_t _wgm_2() {
      return WGM02;
    }
  };
}

// because clock is on Timer0 we don't allow PhaseCorrectPwm
template <_timer::OutputMode OutModeA, _timer::OutputMode OutModeB, uint8_t SetDDR_A, uint8_t SetDDR_B, typename TaskA, typename TaskB>
class Pwm<_timer::Timer0, _timer::PhaseCorrectPwm, OutModeA, OutModeB, SetDDR_A, SetDDR_B, TaskA, TaskB>;
// by providing no implementation compilation will fail if this combination (Timer0, PhaseCorrectPwm) is requested

#define OVF_TASK(name) \
struct name { \
  static inline uint8_t is_enabled() { return 1; } \
  template <typename I> \
  static inline void handle(I); \
}; \
template <typename I> \
inline void name::handle(I)
