#pragma once

namespace _timer0 {
  // because clock is on Timer0 we don't allow PhaseCorrectPwm
  template <_timer::ClockSelect ClockSelect>
  struct ModeSupport<_timer::Mode::PhaseCorrectPwm, ClockSelect>;
  
  template <_timer::ClockSelect ClockSelect>
  struct ModeSupport<_timer::Mode::PhaseCorrectPwmOcrA, ClockSelect>;
  // if requested compilation will fail
}