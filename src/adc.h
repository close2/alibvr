#pragma once

#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

#include "ports.h"
#include "irqs.h"
#include "type_traits.h"


namespace _adc {
  enum class Mode {
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
  
  enum class Ref {
    ARef = 0b00,
    AVcc = 0b01,
    V1_1 = 0b11
  };
  
  namespace Input {
    struct Temperature {
      static const enum _ports::Port port = _ports::Port::C;
      static const uint8_t bit = 8;
    };
  
    struct V1_1 {
      static const enum _ports::Port port = _ports::Port::C;
      static const uint8_t bit = 14;
    };
    
    struct Gnd {
      static const enum _ports::Port port = _ports::Port::C;
      static const uint8_t bit = 15;
    };
    
    struct Unset {
      static const enum _ports::Port port;
      static const uint8_t bit;
    };
  }

  // forward declaration
  template <uint8_t goto_sleep_for_noise_reduction>
  void _do_adc();
  
  
  void busy_wait_adc_finished() {
    loop_until_bit_is_clear(ADCSRA, ADSC);
  }
  
  // forward declaration which will be defined when irq handler is registered.
  // (in header file)
  void irq_handler_for_adc_must_be_registered_for_noise_reduction();
  
  typedef void(* task)(const uint16_t&);
  
  static void do_nothing(const uint16_t& result) {}
}

template <_adc::Ref  DefaultRef   = _adc::Ref::AVcc,
          typename   DefaultInput = _adc::Input::Unset,
          _adc::Mode DefaultMode  = _adc::Mode::SingleConversion,
          _adc::task Task         = _adc::do_nothing>
class Adc {
private:
  constexpr static uint8_t _calc_prescaler() {
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
  template <typename   Input = DefaultInput,
            _adc::Ref  Ref   = DefaultRef,
            _adc::Mode Mode  = DefaultMode>
  static void init() {
    static_assert(Input::port == _ports::Port::C && (Input::bit == 0 ||
                                                     Input::bit == 1 ||
                                                     Input::bit == 2 ||
                                                     Input::bit == 3 ||
                                                     Input::bit == 4 ||
                                                     Input::bit == 5 ||
                                                     Input::bit == _adc::Input::Temperature::bit ||  // Temperature
                                                     Input::bit == _adc::Input::V1_1::bit || // V1_1
                                                     Input::bit == _adc::Input::Gnd::bit),  // Gnd
                  "Only ADC0-ADC5, _adc::Input::Temperature, _adc::Input::V1_1 and _adc::Input::Gnd are acceptable inputs");
    
    static_assert(Input::port != _ports::Port::C || Input::bit != _adc::Input::Temperature::bit ||
                  Ref == _adc::Ref::V1_1,
                  "If input is temperature only V1_1 gives meaningful results.");
    
    PRR &= ~(_BV(PRADC)); // disable Power Reduction ADC

    const uint8_t ref = ((uint8_t) Ref) << REFS0;
    const uint8_t source = Input::bit << MUX0;
    ADMUX = ref // set ref
            | source;    // set source
    if (Mode != _adc::Mode::SingleConversion) {
      ADCSRB |= (uint8_t) Mode << ADTS0;
      ADCSRA |= _BV(ADATE);
    }
    
    ADCSRA = _BV(ADEN) // turn ADC power on
             | _calc_prescaler() << ADPS0; // set prescaler
             
    const uint8_t is_do_nothing_task = _adc::do_nothing == Task;
    if (!is_do_nothing_task) ADCSRA |= _BV(ADIE);
  }
  
  static void turn_off() {
    // disable ADC interrupts and turn off power for ADC
    ADCSRA &= ~(_BV(ADIE) | _BV(ADEN));
    PRR |= _BV(PRADC); // enable Power Reduction ADC
  }
  
  static void start_adc_8bit() {
    busy_wait_adc_finished();
    ADMUX |= _BV(ADLAR); // 8bit → left adjusted
    
    ADCSRA |= _BV(ADSC);
  }
  
  static void start_adc_10bit() {
    busy_wait_adc_finished();
    ADMUX &= ~(_BV(ADLAR)); // 10bit → right adjusted
    
    ADCSRA |= _BV(ADSC);
  }
  
  static uint8_t is_adc_finished() {
    return (ADCSRA & _BV(ADSC)) == 0;
  }
  
  static void busy_wait_adc_finished() {
    _adc::busy_wait_adc_finished();
  }
  
  static uint8_t get_adc_8bit_result() {
    return ADCH; // left adjusted → the 8 most significant bits are in ADCH!
  }
  
  static uint16_t get_adc_10bit_result() {
    // gcc isn't allowed to change order of instructions because ADCL and
    // ADCH are marked as volatile.
    // http://www.nongnu.org/avr-libc/user-manual/optimization.html
    uint16_t res = ADCL;
    return res | ADCH << 8;
  }
  
  template<uint8_t goto_sleep_for_noise_reduction = 0>
  static uint8_t adc_8bit() {
    busy_wait_adc_finished();
    ADMUX |= _BV(ADLAR); // 8bit → left adjusted

    // start conversion:
    _adc::_do_adc<goto_sleep_for_noise_reduction>();

    return get_adc_8bit_result();
  }
  
  template<uint8_t goto_sleep_for_noise_reduction = 0>
  static uint16_t adc_10bit() {
    busy_wait_adc_finished();
    ADMUX &= ~(_BV(ADLAR)); // 10bit → right adjusted
    
    // start conversion:
    _adc::_do_adc<goto_sleep_for_noise_reduction>();
    
    return get_adc_10bit_result();
  }
  
  
  static inline void handle(const enum _irqs::Irq i) {
    // instead of having a separate flag to remember if a 10bit or 8bit adc
    // had been requested, we simply look at the left/right adjusted flag
    if (ADMUX & _BV(ADLAR)) {
      // 8 bit (left adjusted)
      //Task::adc_complete(get_adc_8bit_result());
      Task(get_adc_8bit_result());
    } else {
      // 10 bit (right adjusted)
      //Task::adc_complete(get_adc_10bit_result());
      Task(get_adc_10bit_result());
    }
  }
};

namespace _adc {
  
  template <uint8_t goto_sleep_for_noise_reduction>
  void _do_adc() {
    irq_handler_for_adc_must_be_registered_for_noise_reduction();
    uint8_t old_adie = ADCSRA & _BV(ADIE);
    
    if (!old_adie) ADCSRA |= _BV(ADIE);
    
    set_sleep_mode(SLEEP_MODE_IDLE);
    
    // Enable irq
    NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE) {
      // By calling sleep_mode avr goes to sleep and automatically starts
      // conversion.
      sleep_mode();
      
      // We might wake up because of another IRQ.  In that case simple wait
      // until adc has finished.
      busy_wait_adc_finished();
    }
    
    // Restore ADIE if it wasn't enabled.  Wait until adc has finished.
    // Otherwise IRQ might not be called if CPU wakes up because of another
    // IRQ.
    if (!old_adie) {
      ADCSRA &= ~(_BV(ADIE));
    }
  }
  
  template <>
  void _do_adc<0>() {
    // start conversion:
    ADCSRA |= _BV(ADSC);
    busy_wait_adc_finished();
  }
}

#define REGISTER_ADC "internal/register_adc.h"

