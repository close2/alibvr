#pragma once

#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include "clock.h"
#include "ports.h"
//#include "internal/timer.h"
#include "irqs.h"

#ifndef ALIBVR_NAMESPACE_BUTTONS
#  ifdef ALIBVR_NAMESPACE_PREFIX
#    define ALIBVR_NAMESPACE_BUTTONS ALIBVR_NAMESPACE_PREFIX ## buttons
#  else
#    define ALIBVR_NAMESPACE_BUTTONS buttons
#  endif
#endif

using namespace ALIBVR_NAMESPACE_PORTS;
using namespace ALIBVR_NAMESPACE_CLOCK;

namespace ALIBVR_NAMESPACE_BUTTONS {
  
  const uint8_t default_ms_per_round = 5;
  const uint8_t default_long_press_rounds = 300 / default_ms_per_round;
  const uint8_t default_block_rounds = 10;
  
  enum class State {
    Released,
    Pressed,
    LongReleased,
    LongPressed
  };
  
  enum Type {
    Pressed_High,
    Pressed_Low
  };
  
  enum InitIo {
    No,
    HighZ,
    PullUp
  };
  
  
  template <typename Pin>
  static inline decltype(PCMSK0)& _pcmsk() {
    if (Pin::port == Port::B) {
      return PCMSK0;
    }
    if (Pin::port == Port::C) {
      return PCMSK1;
    }
    if (Pin::port == Port::D) {
      return PCMSK2;
    }
    static_assert(Pin::port == Port::B ||
                  Pin::port == Port::C ||
                  Pin::port == Port::D,
                  "Port invalid for retrieval of PCMSK*");
    // should never reach this
    return PCMSK0;
  }
  
  template <typename ...Pins>
  class PinList {
  };
  
  struct DoNothingMatrixListener {
    template <typename Row, typename Column>
    static void state_changed(State, uint8_t, uint8_t) {}
  };

  template <uint8_t ms_per_round,
            uint8_t long_press_rounds,
            uint8_t block_rounds,
            typename... ButtonList>
  class ButtonsWTimes {
  
  private:
    static const uint8_t _released = 0;
    static const uint8_t _pressed = 1;
    // by default 300ms and bit0 is used as indicator for Released / Pressed
    static const uint8_t _long_released = long_press_rounds;
    // by default 300ms and bit0 is used as indicator for Released / Pressed
    static const uint8_t _long_pressed = long_press_rounds + 1;
    
    // from http://en.wikipedia.org/wiki/Variadic_Templates
    struct pass {
      template<typename ...Args> pass(Args...) {}
    };
    
    template <typename Button>
    static inline void _init_button(uint8_t& pcifr, uint8_t& pcmsk0, uint8_t& pcmsk1, uint8_t& pcmsk2) {
      typedef typename Button::Pin Pin;
      if (Button::InitIo == InitIo::HighZ) {
        Pin::setToInput(PullUp::HighZ);
      } else if (Button::InitIo == InitIo::PullUp) {
        Pin::setToInput(PullUp::On);
      }
      if (Pin::port == Port::B) {
        pcifr |= _BV(PCIF0); // enable irq for logic change on port B
      }
      if (Pin::port == Port::C) {
        pcifr |= _BV(PCIF1); // enable irq for logic change on port C
      }
      if (Pin::port == Port::D) {
        pcifr |= _BV(PCIF2); // enable irq for logic change on port D
      }
      // enable Button bit in pin change mask for port B
      set_8_bits<Pin, 0>(pcmsk0, pcmsk1, pcmsk2, 0xFF);
    }
    
    // every button has its own state (uint8_t)
    // bit 0 is used for button down or up state
    // all other bits are counters.
    // → 0b01010001 is button pressed
    //   0b01010000 is button up
    template <typename Button>
    static inline uint8_t _get_state() {
      static uint8_t state = (uint8_t) State::Released;
      return state;
    }
    
    template <class I, typename Button>
    static inline void _handle() {
      typedef typename Button::Pin Pin;
      if (Pin::port == Port::B && I::irq != _irqs::IRQ_PCINT0) return;
      if (Pin::port == Port::C && I::irq != _irqs::IRQ_PCINT1) return;
      if (Pin::port == Port::D && I::irq != _irqs::IRQ_PCINT2) return;
      
      auto& pcmsk = _pcmsk<Button::Pin>();
      if (!(pcmsk & Pin::bit)) return;  // irq for this bit is disabled
  
      const uint8_t p = Pin::PORT;
      const uint8_t pin_now = Button::Type == Type::Pressed_High ? p : !p;
      uint8_t& pin_state = _get_state<Button>();
  
      if ((pin_now && (pin_state & 1)) || (!pin_now && !(pin_state & 1))) return; // state hasn't changed
      
      State new_state = (!pin_now) ? State::Released : State::Pressed;
      
      // don't act on any more irqs for some time (see run())
      pcmsk &= ~_BV(Pin::bit);
  
      pin_state = (uint8_t)new_state;
      Button::state_changed(new_state);
    }
    
    template <typename Button>
    static inline void _update_state() {
      typedef typename Button::Pin Pin;
  
      uint8_t& state = _get_state<Button>();
      if (state == 0xFF || state == 0xFE) return; // don't overflow
      
      state += 2; // keep bit0 untouched!
      
      if ((state / 2) == block_rounds) {
        auto& pcmsk = _pcmsk<Button::Pin>();
        pcmsk |= _BV(Pin::bit);
        
        // call handle, just in case if button really has changed already 
        if (Pin::port == Port::B) _handle<_irqs::IRQ_PCINT0, Button>();
        
        if (Pin::port == Port::C) _handle<_irqs::IRQ_PCINT1, Button>();
        
        if (Pin::port == Port::D) _handle<_irqs::IRQ_PCINT1, Button>();
      }
      if (state == (uint8_t) State::LongPressed) Button::state_changed(State::LongPressed);
      if (state == (uint8_t) State::LongReleased) Button::state_changed(State::LongReleased);
    }
    
  public:
    
    static void init() {
      uint8_t pcifr = 0;
      uint8_t pcmsk0 = 0;
      uint8_t pcmsk1 = 0;
      uint8_t pcmsk2 = 0;
      
      // go through all Button|s and set corresponding bits in PCIFC and PCMSK
      pass{(_init_button<ButtonList>(pcifr, pcmsk0, pcmsk1, pcmsk2), 1)...};
      
      // PCIFR (Pin Change Interrupt Flag Register)
      if (pcifr) PCIFR |= pcifr;
      
      // and in PCMSK* (PinChangeEnableMask)
      if (pcmsk0) PCMSK0 |= pcmsk0;
      if (pcmsk1) PCMSK1 |= pcmsk1;
      if (pcmsk2) PCMSK2 |= pcmsk2;
      sei();
    }
    
    // implement IRQ-handler interface
    // this handle function handles (possibly) 3 irqs PCINT0, PCINT1 and PCINT2
    template <class I>
    static inline void handle(I) {
      pass{(_handle<I, ButtonList>(), 1)...};
    }
    
    template<typename T>
    static inline T run(T) {
      // go through all Buttons, and ++ if pressed, and / or
      // allow irq handling again.
      pass{(_update_state<ButtonList>(), 1)...};
      return ms_to_units<ms_per_round>();
    }
  };
  
  template <typename... ButtonList>
  using Buttons = ButtonsWTimes<default_ms_per_round,
                                default_long_press_rounds,
                                default_block_rounds,
                                ButtonList...>;
                                
                                  
  template <uint8_t ms_per_round,
            uint8_t long_press_rounds,
            uint8_t block_rounds,
            typename RowList,
            typename ColumnList,
            typename Listener = DoNothingMatrixListener>
  class MatrixButtonsWTimes {
    
  private:
    
    // from http://en.wikipedia.org/wiki/Variadic_Templates
    struct pass {
      template<typename ...Args> pass(Args...) {}
    };
    
    // every button has its own state (uint8_t)
    // bit 0 is used for button down or up state
    // all other bits are counters.
    // → 0b01010001 is button pressed
    //   0b01010000 is button up
    template <typename PinRow, typename PinColumn>
    static inline uint8_t& get_state() {
      static uint8_t state = (uint8_t) State::Released;
      return state;
    }
  
    // go through all Pins, and ++ if pressed
    template <typename Row, typename Column>
    static inline void _update_state(uint8_t row, uint8_t& _column) { // _column will be modified!
      const uint8_t column = _column;
      _column++;
      uint8_t& state = get_state<Row, Column>();
      
      // check for new state
      const uint8_t pin_now = !Column::PIN;
      
      if ((state / 2) < block_rounds) {
        state += 2;
        return;
      }
      
      const uint8_t pin_changed = (pin_now && !(state & 1)) || (!pin_now && (state & 1));
      if (!pin_changed) {
        if (state == 0xFF || state == 0xFE) return; // don't overflow
        state += 2; // keep bit0 untouched!
        
        if (state == (uint8_t) State::LongPressed) {
          Listener::template state_changed<Row, Column>(State::LongPressed, row, column);
        } else if (state == (uint8_t) State::LongReleased) {
          Listener::template state_changed<Row, Column>(State::LongReleased, row, column);
        }
        return;
      }
      
      State new_state = (!pin_now) ? State::Released : State::Pressed;
      
      state = (uint8_t) new_state;
      Listener::template state_changed<Row, Column>(new_state, row, column);
    }
   
    template <typename Row, typename ...Columns>
    static inline void _update_states_row(uint8_t& _row) {
      const uint8_t row = _row;
      _row++;
      
      uint8_t column = 0;
      
      Row::DDR = DataDirection::Output;
      _NOP();
      pass{(_update_state<Row, Columns>(row, column), 1)...};
      Row::DDR = DataDirection::Input;
    }
    
    template <typename Column>
    static inline void set_ddr_column() {
      Column::setToInput(PullUp::On);
    }
    
    template <typename Row>
    static inline void set_ddr_row() {
      Row::setToInput(PullUp::HighZ);
    }
    
    template <typename ...Rows, typename ...Columns>
    static inline void _init(PinList<Rows...>, PinList<Columns...>) {
      // set DDR for pins
      // we assume, that we are allowed to do this, because we have to change
      // output values anyways when trying different rows
      pass{(set_ddr_row<Rows>(), 1)...};
      pass{(set_ddr_column<Columns>(), 1)...};
    }
    
    template<typename ...Rows, typename ...Columns>
    static inline void _run(PinList<Rows...>, PinList<Columns...>) {
      uint8_t row = 0;
      pass{(_update_states_row<Rows, Columns>(row), 1)...};
    }
    
  public:
    
    static void init() {
      _init(RowList(), ColumnList());
    }
    
    template<typename T>
    static inline T run(T) {
      _run(RowList(), ColumnList());
      return ms_to_units<ms_per_round>();
    }
  };
  
  template <typename RowList,
            typename ColumnList,
            typename Listener = DoNothingMatrixListener>
  using MatrixButtons = MatrixButtonsWTimes<default_ms_per_round,
                                default_long_press_rounds,
                                default_block_rounds,
                                RowList,
                                ColumnList,
                                Listener>;
}
                                

#define REGISTER_BUTTONS "internal/register_buttons.h"
#define REGISTER_MBUTTONS "internal/register_mbuttons.h"
