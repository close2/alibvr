#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "macros.h"
#include "type_traits.h"

namespace _adc {
  enum Mode {
    SingleConversion     = 0xFF,
    FreeRunning          = 0b000,
    TriggerAnalogComp    = 0b001,
    TriggerExtIrq0       = 0b010,
    TriggerTimer0CompA   = 0b011,
    TriggerTimer0OvF     = 0b100,
    TriggerTimer1CompB   = 0b101,
    TriggerTimer1OvF     = 0b110,
    TriggerTimer1Capture = 0b111
  };
  
  enum Ref {
    ARef = 0b00,
    AVcc = 0b01,
    V1_1 = 0b11
  };
  
  namespace Input {
    struct Temperature {
      static const enum _macros::Port port = _macros::Port::B;
      static const uint8_t bit = 8;
    };
  
    struct V1_1 {
      static const enum _macros::Port port = _macros::Port::B;
      static const uint8_t bit = 14;
    };
    
    struct Gnd {
      static const enum _macros::Port port = _macros::Port::B;
      static const uint8_t bit = 15;
    };
  }

  struct DoNothing {
    static uint8_t is_enabled() { return false; }
    template <typename size_t>
    static void adc_complete(const size_t& result) {};
  };

}

template <_adc::Mode           Mode,
          _adc::Ref            Ref,
          typename             Input,
          typename             Task = _adc::DoNothing>
class Adc {
private:
  constexpr static uint8_t calc_prescaler() {
    // is F_CPU divided by n (prescaler) below 200kHz? then prescaler is ok
    // returns bits for ADPS2, ADPS1, ADPS0
    return (F_CPU /   2) <= 200000 ? 0b000 :
           (F_CPU /   4) <= 200000 ? 0b010 :
           (F_CPU /   8) <= 200000 ? 0b011 :
           (F_CPU /  16) <= 200000 ? 0b100 :
           (F_CPU /  32) <= 200000 ? 0b101 :
           (F_CPU /  64) <= 200000 ? 0b110 :
           (F_CPU / 128) <= 200000 ? 0b111 :
           0b000;  // if nothing works only divide by 2 (safest prescaler)
  }
  
public:
  // the first adc is not guaranteed to have a meaningful value
  static void init(uint8_t adc_and_discard_first = 1) {
    ADCSRA = _BV(ADEN) // turn ADC power on
             | calc_prescaler() << ADPS0; // set prescaler

    PRR &= ~(_BV(PRADC)); // disable Power Reduction ADC

    static_assert(Input::port == _macros::Port::C && (Input::bit == 0 ||
                                                      Input::bit == 1 ||
                                                      Input::bit == 2 ||
                                                      Input::bit == 3 ||
                                                      Input::bit == 4 ||
                                                      Input::bit == 5 ||
                                                      Input::bit == 8 ||  // Temperature
                                                      Input::bit == 14 || // V1_1
                                                      Input::bit == 15),  // Gnd
                  "Only ADC0-ADC5, _adc::Input::Temperature, _adc::Input::V1_1 and _adc::Input::Gnd are acceptable inputs");

    uint8_t source = Input::bit << MUX0;
    ADMUX = Ref << REFS0 // set ref
            | source;    // set source
    if (adc_and_discard_first) {
      ADCSRA |= _BV(ADSC);
    }
  }
  
  static void turn_off() {
    // disable ADC interrupts and turn off power for ADC
    ADCSRA &= ~(_BV(ADIE) | _BV(ADEN));
    PRR |= _BV(PRADC); // enable Power Reduction ADC
  }
  
  static void start_adc_8bit() {
    busy_wait_adc_finished();
    ADMUX |= _BV(ADLAR); // 8bit → left adjusted
    
    uint8_t do_nothing_task = std::is_same<_adc::DoNothing, Task>();
    if (do_nothing_task) {
      // start conversion:
      ADCSRA |= _BV(ADSC);
    } else {
      // enable irq and start
      sei();
      ADCSRA |= _BV(ADIE) | _BV(ADSC);
    }
  }
  
  static void start_adc_10bit() {
    busy_wait_adc_finished();
    ADMUX &= ~(_BV(ADLAR)); // 10bit → right adjusted
    
    uint8_t do_nothing_task = std::is_same<_adc::DoNothing, Task>();
    if (do_nothing_task) {
      // start conversion:
      ADCSRA |= _BV(ADSC);
    } else {
      // enable irq and start
      sei();
      ADCSRA |= _BV(ADIE) | _BV(ADSC);
    }
  }
  
  static void prepare_adc_8bit_noise_reduction() {
    busy_wait_adc_finished();
    ADMUX |= _BV(ADLAR); // 8bit → left adjusted
    
    // enable irq
    sei();
    ADCSRA |= _BV(ADIE);
  }
  
  static void prepare_adc_10bit_noise_reduction() {
    busy_wait_adc_finished();
    ADMUX &= ~(_BV(ADLAR)); // 10bit → right adjusted
    
    // enable irq
    sei();
    ADCSRA |= _BV(ADIE);
  }
  
  static void sleep_idle() {
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_mode();
  }
  
  static uint8_t adc_finished() {
    return (ADCSRA & _BV(ADSC)) == 0;
  }
  
  static void busy_wait_adc_finished() {
    loop_until_bit_is_clear(ADCSRA, ADSC);
  }
  
  static uint8_t get_adc_8bit_result() {
    return ADCH; // left adjusted → the 8 most significant bits are in ADCH!
  }
  
  static uint16_t get_adc_10bit_result() {
    // first read ADCL (and hope gcc doesn't optimize this in some way..)
    uint16_t res = ADCL;
    return res | ADCH << 8;
  }
  
  static uint8_t adc_8bit() {
    busy_wait_adc_finished();
    ADMUX |= _BV(ADLAR); // 8bit → left adjusted
    // start conversion:
    ADCSRA |= _BV(ADSC);
    busy_wait_adc_finished();
    return get_adc_8bit_result();
  }
  
  static uint16_t adc_10bit() {
    busy_wait_adc_finished();
    ADMUX &= ~(_BV(ADLAR)); // 10bit → right adjusted
    // start conversion:
    ADCSRA |= _BV(ADSC);
    busy_wait_adc_finished();
    
    return get_adc_10bit_result();
  }
  
  
  template <typename I>
  static inline void handle(I) {
    uint8_t do_nothing_task = std::is_same<_adc::DoNothing, Task>();
    if (!do_nothing_task) {
      // instead of having a separate flag to remember if a 10bit or 8bit adc
      // had been requested, we simply look at the left/right adjusted flag
      if (ADMUX & _BV(ADLAR)) {
        // 8 bit (left adjusted)
        Task::adc_complete(get_adc_8bit_result());
      } else {
        // 10 bit (right adjusted)
        Task::adc_complete(get_adc_10bit_result());
      }
    }
  }
};

#define REGISTER_ADC "internal/register_adc.h"