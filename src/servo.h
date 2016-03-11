#pragma once

#include "clock.h"
#include "limits.h"
#include "internal/timer.h"
#include <util/delay.h>

/* BALI NOTES
 * - how to use / what is ServoPause
 *   AFAICR a servo needs a high every n ms and ServoPause should make sure
 *   that this pause exists: 111222222233ppppp111222222233pppppp
 *   where numbers represent highs for the servo n
 * - define default for ServoPause PauseLength
 * - how many servos can we handle?
 * - how precise is our timing?
 * - Note: All servos are processed sequentially.
 * - Improve doc: get_ulse_length units are us.
 * - Provide wrapper class (for the necessary interface see ServoPause)
 * - Make task interface compatible to timer-task.
 *   template<typename T, typename Timer = Timer0, _timer::Unit U = A>
 *                ↑ add this to timer-task (irq) interface and provide defaults
 */

namespace _servo {
  
  const uint16_t no_remaining = 0xFFFF;
  
  template <uint16_t PauseLength>
  class ServoPause {
    public:
      static void on() {
        // do nothing
      }
      static void off() {
        // do nothing
      }
      static uint16_t get_pulse_length() {
        return PauseLength;
      }
  };
}

template <typename... ServoList>
class Servos {

private:
  
  // from http://en.wikipedia.org/wiki/Variadic_Templates
  struct pass {
    template<typename ...Args> pass(Args...) {}
  };
  
  template <typename Timer>
  constexpr static typename Timer::size_t half_round() {
    return std::numeric_limits<typename Timer::size_t>::max() / 2;
  }
  
  template<typename Timer, typename Servo>
  static inline void handle1Servo(uint16_t& remaining_us, uint8_t& counter, uint8_t& currentServo, const typename Timer::size_t& prev_ocr, typename Timer::size_t& next_ocr_in) {
    const uint16_t half_round_units = half_round<Timer>();
    
    // these should be calculated by the compiler
    const uint32_t half_round_us = units_to_us<Timer::ClockSelect>(half_round_units);
    const uint32_t complete_round_us = 2 * half_round_us;
    const uint32_t one_and_half_round_us = half_round_us + complete_round_us;
    
    if (counter++ == currentServo) {
      if (remaining_us == _servo::no_remaining) {
        Servo::on();
        const uint16_t pulse_length = Servo::get_pulse_length();
        remaining_us = pulse_length;
      }
      if (remaining_us != 0) {
        if (remaining_us > one_and_half_round_us) {
          next_ocr_in = 0;  // let timer make a complete round
          remaining_us -= complete_round_us;
        } else if (remaining_us > complete_round_us) {
          next_ocr_in = half_round_units; // let timer make a half round
          remaining_us -= half_round_us;
        } else {
          next_ocr_in = us_to_units<Timer::ClockSelect>(remaining_us);
          remaining_us = 0; // after this round we are done
        }
      } else {
        Servo::off();
        remaining_us = _servo::no_remaining;
        currentServo++;  // we are done with this servo
      }
    }
  }

public:
  
  template <typename Timer, _timer::Unit U>
  static typename Timer::size_t run(const typename Timer::size_t prev_ocr) {
    
    static_assert(Timer::TimerMode == _timer::Mode::Normal, "Servo is only possible if Timer is in Normal mode");
    static_assert(sizeof...(ServoList) > 0, "You have not provided any Servo definitions");
    
    static uint8_t staticCurrentServo = 0;
    uint8_t currentServo = staticCurrentServo;
    
    static uint16_t remaining_us = _servo::no_remaining; // 
    
    typename Timer::size_t new_ocr = 0;
    do { // allow restart if no pause is needed and all servos have been handled
      
      uint8_t counter = 0;
      
      // note the Brace-enclosed initializer list constructor
      pass{(handle1Servo<Timer, ServoList>(remaining_us, counter, currentServo, prev_ocr, new_ocr), 1)...};
      if (currentServo == sizeof...(ServoList)) { // done with real servos
        currentServo = 0;
        
        // restart, no pause is needed
        continue;
      }
      break;
      
    } while (1);
    staticCurrentServo = currentServo;
    return new_ocr;
  } 
  
  static uint8_t is_enabled() {
    return 1;
  }
};