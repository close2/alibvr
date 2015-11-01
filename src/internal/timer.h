#pragma once

#include <avr/interrupt.h>

// TODO MALTA NOTES: move following 3 files into internal
//                   public version will be includes
#include "../type_traits.h"
#include "../macros.h"
#include "../clock_select.h"
#include "clock_prescale.h"

namespace _timer {
  
  enum class Unit {
    A,
    B
  };

  struct DoNothing {
    static uint8_t is_enabled() { return false; }
    template <typename Timer, Unit U>
    static uint8_t run(const uint8_t& clock) { return 0; }
    static void run() {};
    template <typename I>
    static inline void handle(I) {}
  };

  enum class Mode {
    Normal = 0b0000,
    Ctc = 0b0100,
    CtcIcr = 0b1100,
    // pwm:
    FastPwm = 0b0101,
    FastPwm9 = 0b0110,
    FastPwm10 = 0b0111,
    FastPwmIcr = 0b1110,
    FastPwmOcrA = 0b1111,
    PhaseCorrectPwm = 0b0001,
    PhaseCorrectPwm9 = 0b0010,
    PhaseCorrectPwm10 = 0b0011,
    PhaseCorrectPwmIcr = 0b1010,
    PhaseCorrectPwmOcrA = 0b1011,
    PhaseAndFrequencyCorrectPwm = 0b1000,
    PhaseAndFrequencyCorrectPwmIcr = 0b1000,
    PhaseAndFrequencyCorrectPwmOcrA = 0b1001
  };
  
  enum class OutputMode {
    Disconnected,
    Toggle,
    ClearOnMatch,
    SetOnMatch,
    // pwm
    NonInverting = ClearOnMatch,
    Inverting = SetOnMatch
  };
  
  typedef _clock_select::ClockSelect ClockSelect;
  
  static const ClockSelect ClockPrescale = (ClockSelect)_clock::Prescale;
  
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
  struct UserDef {
    static const auto Mode = _Mode;
    static const auto ClockSelect = _ClockSelect;
    static const auto OutModeA  = _OutModeA;
    static const auto OutModeB  = _OutModeB;
    static const auto SetDDR_A = _SetDDR_A;
    static const auto SetDDR_B = _SetDDR_B;
    typedef _TaskA    TaskA;
    typedef _TaskB    TaskB;
    typedef _TaskOvF  TaskOvF;
  };
  
  template <typename _TimerDef, typename _UserDef>
  class Timer {
  public:
    typedef _UserDef  UserDef;
    typedef _TimerDef TimerDef;
    
    typedef Timer<TimerDef, UserDef> this_t;
    typedef typename TimerDef::size_t size_t;
    
    static const auto TimerMode = UserDef::Mode;
    static const auto ClockSelect = UserDef::ClockSelect;

  private:
    
    typedef typename UserDef::TaskA   TaskA;
    typedef typename UserDef::TaskB   TaskB;
    typedef typename UserDef::TaskOvF TaskOvF;
  
    template <uint8_t _com_1, uint8_t _com_0, uint8_t SetDDR, typename PIN, uint8_t ForceSetOcr, _timer::OutputMode OutMode, typename R, typename size_t>
    static inline void init_unit(R& tccra,
                                 R& tccrb,
                                 R& tcnt,
                                 size_t& ocr,
                                 R& timsk,
                                 const size_t& initial_pwm) {
    }
    
    template <_timer::Unit U, uint8_t SetDDR, typename PIN, typename _timer::OutputMode OutMode, typename T, typename R>
    static inline void _init_unit(R& tccra,
                                  R& tccrb,
                                  R& tcnt,
                                  size_t& ocr,
                                  R& timsk,
                                  const size_t& initial_pwm) {
      const uint8_t doNothingTask = std::is_same<T, _timer::DoNothing>();
      if (OutMode == _timer::OutputMode::Disconnected && doNothingTask) return;

      ocr = initial_pwm;
      const auto _com_1 = TimerDef::template _com_1<U>();
      const auto _com_0 = TimerDef::template _com_0<U>();

      switch (OutMode) {
        case _timer::OutputMode::Disconnected:
          // set them both to 0
          tccra &= ~(_BV(_com_1) | _BV(_com_0));
          break;
        case _timer::OutputMode::Toggle:
          if (SetDDR) SET_BIT(PIN, DDR, 1);
          tccra &= ~(_BV(_com_1));
          tccra |= _BV(_com_0);
          break;
        case _timer::OutputMode::Inverting:
          if (SetDDR) SET_BIT(PIN, DDR, 1);
          tccra |= _BV(_com_1) | _BV(_com_0);
          break;
        case _timer::OutputMode::NonInverting:
          if (SetDDR) SET_BIT(PIN, DDR, 1);
          tccra |= _BV(_com_1);
          tccra &= ~(_BV(_com_0));
          break;
      }

      if (! doNothingTask) timsk |= _BV(TimerDef::template _ocie<U>());
    }
    
    template <typename R>
    static inline void _init_irq_ovf(R& timsk) {
      const uint8_t doNothing = std::is_same<TaskOvF, _timer::DoNothing>();
      if (! doNothing) {
        timsk |= _BV(TimerDef::_toie());
      }
    }
  
  public:
  
    template <typename I>
    static inline void handle(I) {
      if (I::irq == TimerDef::IRQ_A) {
        if (TaskA::is_enabled()) {
          const size_t prev_pwm = get_ocr_A();
          const size_t next_pwm = TaskA::template run<this_t, _timer::Unit::A>(prev_pwm);
          if (next_pwm != 0) set_ocr_A(prev_pwm + next_pwm);
        }
      }
      if (I::irq == TimerDef::IRQ_B) {
        if (TaskB::is_enabled()) {
          const size_t prev_pwm = get_ocr_B();
          const size_t next_pwm = TaskB::template run<this_t, _timer::Unit::B>(prev_pwm);
          if (next_pwm != 0) set_ocr_B(prev_pwm + next_pwm);
        }
      }
      if (I::irq == TimerDef::IRQ_OVF) {
        if (TaskOvF::is_enabled()) {
          TaskOvF::handle(I());
        }
      }
    }
    
    static inline void init(const size_t& initial_ocr_A,
                            const size_t& initial_ocr_B) {
      uint8_t tcnt = 0;
      uint8_t timsk = 0;
      uint8_t tccra = 0;
      uint8_t tccrb = 0;
      size_t ocr_A = 0;
      size_t ocr_B = 0;
      
      _init_unit<Unit::A, UserDef::SetDDR_A, typename TimerDef::PIN_A, UserDef::OutModeA, TaskA>(tccra, tccrb, tcnt, ocr_A, timsk, initial_ocr_A);
      _init_unit<Unit::B, UserDef::SetDDR_B, typename TimerDef::PIN_B, UserDef::OutModeB, TaskB>(tccra, tccrb, tcnt, ocr_B, timsk, initial_ocr_B);
      _init_irq_ovf(timsk);
     
      TimerDef::template init<this_t>(tcnt, timsk, tccra, tccrb, ocr_A, ocr_B, initial_ocr_A, initial_ocr_B);
      
      // now assign those calculated values to the real registers
      if (tcnt)  TimerDef::tcnt() = tcnt;
      if (timsk) TimerDef::timsk() = timsk; 
      if (tccra) TimerDef::tccra() = tccra;
      if (tccrb) TimerDef::tccrb() = tccrb;
      if (ocr_A) TimerDef::template ocr<Unit::A>() = ocr_A;
      if (ocr_B) TimerDef::template ocr<Unit::B>() = ocr_B;
      sei();
    }
    
    template <Unit U>
    static inline size_t get_ocr() {
      return TimerDef::template ocr<U>();
    }
    static inline size_t get_ocr_A() { return get_ocr<Unit::A>(); }
    static inline size_t get_ocr_B() { return get_ocr<Unit::B>(); }
    template <Unit U>
    static inline size_t get_pwm() { return get_ocr<U>(); }
    static inline size_t get_pwm_A() { return get_pwm<Unit::A>(); }
    static inline size_t get_pwm_B() { return get_pwm<Unit::B>(); }

    template <Unit U>
    static inline void set_ocr(const size_t& new_ocr) {
      TimerDef::template ocr<U>() = new_ocr;
    }
    static inline void set_ocr_A(const size_t& new_ocr) { set_ocr<Unit::A>(new_ocr); }
    static inline void set_ocr_B(const size_t& new_ocr) { set_ocr<Unit::B>(new_ocr); }
    template <Unit U>
    static inline void set_pwm(const size_t& new_pwm) { set_ocr<U>(new_pwm); }
    static inline void set_pwm_A(const size_t& new_pwm) { set_pwm<Unit::A>(new_pwm); }
    static inline void set_pwm_B(const size_t& new_pwm) { set_pwm<Unit::B>(new_pwm); }

    template <Unit U>
    static inline void enable_match_task() {
      TimerDef::template timsk() |= _BV(TimerDef::template _ocie<U>());
    }
    static inline void enable_match_task_A() { enable_match_task<Unit::A>(); }
    static inline void enable_match_task_B() { enable_match_task<Unit::B>(); }

    template <Unit U>
    static inline void disable_match_task() {
      TimerDef::template timsk() &= ~_BV(TimerDef::template _ocie<U>());
    }
    static inline void disable_match_task_A() { disable_match_task<Unit::A>(); }
    static inline void disable_match_task_B() { disable_match_task<Unit::B>(); }
  };
}
