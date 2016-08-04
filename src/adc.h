#pragma once

#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

#include "ports.h"
#include "irqs.h"
#include "type_traits.h"


#ifndef ALIBVR_NAMESPACE_ADC
#  ifdef ALIBVR_NAMESPACE_PREFIX
#    define ALIBVR_NAMESPACE_ADC ALIBVR_NAMESPACE_PREFIX ## adc
#  else
#    define ALIBVR_NAMESPACE_ADC adc
#  endif
#else
#  define ALIBVR_NAMESPACE_ADC ALIBVR_NAMESPACE_ADC
#endif


/**
 * 
 * By default adc related classes, enums,... are defined
 * inside the `adc` namespace.  If this creates a name clash with your
 * code you may modify the namespace name by setting
 * ALIBVR_NAMESPACE_ADC or ALIBVR_NAMESPACE_PREFIX.
 * 
 **/

namespace ALIBVR_NAMESPACE_ADC {
  
  /**
   * @brief An AD-conversion is either started manually or by specifying
   * a trigger.
   * 
   * When the selected trigger (a bit in a register) changes from 0 to 1,
   * a conversion is started.
   * 
   * All ADC triggers are IRQ bits.  The TriggerTimer0OvF for instance
   * starts a new conversion whenever the Irq flag for the Timer0
   * Overflow is changed from 0 to 1.
   * 
   * If the specific irq is enabled and global irqs are turned on the
   * corresponding irq handler is called.  (This behaviour is
   * independent of the ADC subsystem.)  Remember to clear the flag
   * if you don't have irqs enabled, as the trigger only activates when
   * the bit changes from 0 to 1!
   * 
   * The ADC subsystem however does not care if an interrupt
   * is raised and how the bit is set / cleared.  It is therefore
   * possible to use the triggers without enabling any irqs at all!
   * 
   * The FreeRunning mode is actually a trigger like the others.
   * In FreeRunning mode an ADC is started whenever the ADC irq flag
   * is set.  This usually means, that the first conversion is started
   * manually.  Then, whenever an adc has finished a new conversion is
   * automatically started, because the adc irq flag is set whenever
   * a conversion finishes.
   * 
   * See irqs for a more detailed description of all irqs.
   **/
  //«ADC_MODES[^  ,]»
  enum class Mode {
    SingleConversion   = 0xFF,
    FreeRunning        = 0b000,
    TriggerAnalogComp  = 0b001,
    TriggerPCInt0      = 0b010,
    TriggerTimer0CompA = 0b011,
    TriggerTimer0OvF   = 0b100,
    TriggerTimer1CompB = 0b101,
    TriggerTimer1OvF   = 0b110,
    TriggerTimer1Capt  = 0b111
  };
  /*¤*/
  
  //«ADC_REFS[^  ,]»
  enum class Ref {
    ARef = 0b00,
    AVcc = 0b01,
    V1_1 = 0b11
  };
  /*¤*/
  
  /**
   * @brief In addition to the ADC-pins the adc subsystem can also use
   * an internal temperature sensor, an internal 1.1V reference voltage
   * and ground as input for a conversion.
   * 
   * You may use Input::Temperature, Input::V1_1 and Input::Gnd as if
   * they were ADC-pins.
   * 
   * The required static fields port and bit are implemented and have
   * special values, so that the code of this library knows when to
   * switch to those inputs.
   **/
  namespace Input {
    /**
     * @brief "The measured voltage has a linear relationship to the
     * temperature [...]. The voltage sensitivity is approximately
     * 1 mV/°C and the accuracy of the temperature measurement is
     * +/- 10°C." (Atmega docs)
     * 
     * Typical values:
     * - -45°C: 242mV
     * - +25°C: 314mV
     * - +85°C: 380mV
     * These values are typical values.
     * 
     * To achieve more accuracy calibrate the temperature sensor by
     * storing a calibration value in the eeprom.  See the atmel
     * docs for more information.
     **/
    struct Temperature {
      static const enum ALIBVR_NAMESPACE_PORTS::_Port port = ALIBVR_NAMESPACE_PORTS::_Port::C;
      static const uint8_t bit = 8;
    };
  
    /**
     * @brief The internal 1.1V bandgap reference voltage.
     * 
     * Minimum 1.0V, Maximum 1.2V
     * 
     * The reference voltage needs some time to stabilize.  Typically
     * 40µs, but it may take up to 70µs until it has stabilized.
     * 
     **/
    struct V1_1 {
      static const enum ALIBVR_NAMESPACE_PORTS::_Port port = ALIBVR_NAMESPACE_PORTS::_Port::C;
      static const uint8_t bit = 14;
    };
    
    /**
     * @brief Ground.
     **/
    struct Gnd {
      static const enum ALIBVR_NAMESPACE_PORTS::_Port port = ALIBVR_NAMESPACE_PORTS::_Port::C;
      static const uint8_t bit = 15;
    };
    
    /**
     * @brief Use Unset during initialization if you intend to use
     * different inputs.  Using Unset doesn't prevent you from
     * using different inputs, but maybe makes your intentations
     * clearer.
     * 
     * TL;DR This library would work without Unset as well, but
     * it might help to understand your code more easily.
     **/
    struct Unset {
      static const enum ALIBVR_NAMESPACE_PORTS::_Port port;
      static const uint8_t bit;
    };
  }

  // forward declaration
  template <uint8_t goto_sleep_for_noise_reduction>
  void _do_adc();
  
  
  /**
   * @brief Loops until the current conversion has finished.
   **/
  void busy_wait_adc_finished() {
    loop_until_bit_is_clear(ADCSRA, ADSC);
  }
  
  // forward declaration which will be defined when irq handler is registered.
  // (in header file)
  void irq_handler_for_adc_must_be_registered_for_noise_reduction();
  
  typedef void(* task)(const uint16_t&);
  
  /**
   * @example adc_2_irqs.cpp.
   **/
  
  /**
   * @brief Static functions for initialization, adc controls and irqs.
   * 
   * If you use different instances of the Adc class the adc subsystem
   * is shared.  Calling init() sets the input, mode or reference for
   * all following conversions.  It is possible to register more than
   * one Adc irq task but _all_ tasks are executed whenever a
   * conversion finishes.
   * 
   * @tparam DefaultRef sets the default reference voltage for conversions.
   *         See Ref for possible values.  The reference voltage can be
   *         changed by passing a different Ref as template argument to
   *         the init() function.
   * @tparam DefaultInput sets the default input.  In addition to the
   *         ADC-pins the classes in the Input namespace can be used.
   *         The input can be changed by passing a different input as
   *         template argument to the init() function.
   * @tparam DefaultMode sets the default mode.  See Mode for possible
   *         values.
   *         The mode can be changed by passing a different mode as
   *         template argument to the init() function.
   * @tparam Task is a function which is executed in the irq handler.
   *         To register the irq handler assign your typedef to the
   *         macro `NEW_ADC` and `#include REGISTER_ADC`.
   *         Then, after your main, `#include REGISTER_IRQS`
   *
   *      
+++ADC_IRQ_REG[^,   *     ]+++
   **/
  template <Ref DefaultRef        = Ref::AVcc,
            typename DefaultInput = Input::Unset,
            Mode DefaultMode      = Mode::SingleConversion,
            task Task             = nullptr>
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
    /**
     * @brief Initialize the adc sub system.
     * 
     * Note that the order of the template arguments is different to
     * the order of the template arguments of Adc!
     * 
     * If power consumption is a concern you have to turn the adc
     * subsystem off after having used it.  See turn_off().
     * 
     * Simply call init() another time to turn it on again.
     * 
     * The first conversion after init() is not guaranteed to have
     * a meaningful value!
     * 
     * @tparam I sets the input.  In addition to the ADC-pins the
     *         classes in the Input namespace can be used.
     *         This template argument overrides the Adc argument.
     * @tparam R sets the reference voltage for conversions.
     *         See Ref for possible values.
     *         This template argument overrides the Adc argument.
     * @tparam M sets the mode.  See Mode for possible values.
     *         This template argument overrides the Adc argument.
     **/
    template <typename I = DefaultInput,
              Ref R      = DefaultRef,
              Mode M     = DefaultMode>
    static void init() {
      static_assert(I::port == ALIBVR_NAMESPACE_PORTS::_Port::C &&
                               (I::bit == 0 ||
                                I::bit == 1 ||
                                I::bit == 2 ||
                                I::bit == 3 ||
                                I::bit == 4 ||
                                I::bit == 5 ||
                                I::bit == Input::Temperature::bit ||  // Temperature
                                I::bit == Input::V1_1::bit || // V1_1
                                I::bit == Input::Gnd::bit),  // Gnd
                    "Only ADC0-ADC5, Input::Temperature, Input::V1_1 and Input::Gnd are acceptable inputs");
      
      static_assert(I::port != ALIBVR_NAMESPACE_PORTS::_Port::C || I::bit != Input::Temperature::bit ||
                    R == Ref::V1_1,
                    "If input is temperature only V1_1 gives meaningful results.");
      
      PRR &= ~(_BV(PRADC)); // disable Power Reduction ADC
  
      const uint8_t ref = ((uint8_t) R) << REFS0;
      const uint8_t source = I::bit << MUX0;
      ADMUX = ref // set ref
              | source;    // set source
      if (M != Mode::SingleConversion) {
        ADCSRB |= (uint8_t) M << ADTS0;
        ADCSRA |= _BV(ADATE);
      }
      
      ADCSRA = _BV(ADEN) // turn ADC power on
               | _calc_prescaler() << ADPS0; // set prescaler
               
      const uint8_t is_do_nothing_task = nullptr == Task;
      if (!is_do_nothing_task) ADCSRA |= _BV(ADIE);
    }
    
    /**
     * @brief Turns the adc subsystem off reducing power consumption.
     * 
     * To turn the system on again, simply call init() again.
     **/
    static void turn_off() {
      // disable ADC interrupts and turn off power for ADC
      ADCSRA &= ~(_BV(ADIE) | _BV(ADEN));
      PRR |= _BV(PRADC); // enable Power Reduction ADC
    }
    
    /**
     * @brief Busywaits for current conversion to finish, then starts
     * a new one with an 8bit resolution.
     * 
     * This function will not wait for the conversion to finish!  Use
     * adc_8bit() instead if this is what you want.
     * 
     * To find out if the conversion has finished call
     * is_adc_finished().  To get the result call
     * get_adc_8bit_result().
     **/
    static void start_adc_8bit() {
      busy_wait_adc_finished();
      ADMUX |= _BV(ADLAR); // 8bit → left adjusted
      
      ADCSRA |= _BV(ADSC);
    }
    
    /**
     * @brief Busywaits for current conversion to finish, then starts
     * a new one with an 10bit resolution.
     * 
     * See start_adc_8bit() for more information.  All *8bit* functions
     * have *10bit* equivalents.
     **/
    static void start_adc_10bit() {
      busy_wait_adc_finished();
      ADMUX &= ~(_BV(ADLAR)); // 10bit → right adjusted
      
      ADCSRA |= _BV(ADSC);
    }
    
    /**
     * @brief Returns true if no conversion is currently running.
     **/
    static uint8_t is_adc_finished() {
      return (ADCSRA & _BV(ADSC)) == 0;
    }
    
    /**
     * @brief Busywaits for current conversion to finish.  Returns
     * immediatly if no conversion is running.
     **/
    static void busy_wait_adc_finished() {
      while (!is_adc_finished());
    }
    
    /**
     * @brief Returns the result of a previous conversion.
     * 
     * Use this function after having performed an 8bit conversion.
     * 
     * There is only one result buffer for both 8bit conversions
     * and 10bit conversions.  Calling this function after a 10bit
     * conversion will not round the 10bit result but return an
     * incorrect value!
     **/
    static uint8_t get_adc_8bit_result() {
      return ADCH; // left adjusted → the 8 most significant bits are in ADCH!
    }
    
    /**
     * @brief Returns the result of a previous conversion.
     * 
     * Use this function after having performed an 10bit conversion.
     * 
     * There is only one result buffer for both 8bit conversions
     * and 10bit conversions.  Calling this function after a 8bit
     * conversion will not return the correct value!
     **/
    static uint16_t get_adc_10bit_result() {
      // gcc isn't allowed to change order of instructions because ADCL and
      // ADCH are marked as volatile.
      // http://www.nongnu.org/avr-libc/user-manual/optimization.html
      uint16_t res = ADCL;
      return res | ADCH << 8;
    }
    
    /**
     * @brief Waits for current conversions to finish, then performs an
     * 8bit conversion and returns the result.
     * 
     * AD-conversions are more precise if performed while the cpu is
     * sleeping.
     * 
     * @tparam goto_sleep_for_noise_reduction If set to 1 will put the
     *         cpu to sleep (idle).  In this sleep mode only the
     *         cpu-clock and flash-clock are turned off, which means
     *         that other irqs (timer,...) might wake the cpu during a
     *         conversion.  This case is ignored by this function.
     *         (Your result will not be as precise as possible.)  \n
     *         Irq handling will be globally turned on before a
     *         conversion and set back to the previous value
     *         afterwards.  \n
     *         You have to register an irq handler!  Even if it is the
     *         nullptr task.  (See the documentation for the Adc
     *         template argument Task)
     **/
    template<uint8_t goto_sleep_for_noise_reduction = 0>
    static uint8_t adc_8bit() {
      busy_wait_adc_finished();
      ADMUX |= _BV(ADLAR); // 8bit → left adjusted

      // start conversion:
      _do_adc<goto_sleep_for_noise_reduction>();
  
      return get_adc_8bit_result();
    }
    
    /**
     * @brief Waits for current conversions to finish, then performs an
     * 10bit conversion and returns the result.
     * 
     * See adc_8bit().
     **/
    template<uint8_t goto_sleep_for_noise_reduction = 0>
    static uint16_t adc_10bit() {
      busy_wait_adc_finished();
      ADMUX &= ~(_BV(ADLAR)); // 10bit → right adjusted
      
      // start conversion:
      _do_adc<goto_sleep_for_noise_reduction>();
      
      return get_adc_10bit_result();
    }
    
    
    /**
     * @brief This is the irq handler which will be registered through
     * `#include REGISTER_ADC`.
     * 
     * Probably only useful internally.
     **/
    static inline void handle(const enum _irqs::Irq i) {
      const uint8_t is_do_nothing_task = nullptr == Task;
      if (is_do_nothing_task) return;
      
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
  
  /**
   * @brief Starts a conversion while going to sleep.  Then busywaits
   * for the conversion to finish.  The busywait is only necessary
   * because other irqs might wake the cpu during a conversion.
   * 
   * There is another specialized _do_adc which does not goto sleep.
   **/
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
  
  /**
   * @brief Simply starts a conversion and then waits for the
   * conversion to finish.
   * 
   * This is the much simpler specialized version of _do_adc when going
   * to sleep is not requested.
   **/
  template <>
  void _do_adc<0>() {
    // start conversion:
    ADCSRA |= _BV(ADSC);
    busy_wait_adc_finished();
  }
}

#define REGISTER_ADC "internal/register_adc.h"
