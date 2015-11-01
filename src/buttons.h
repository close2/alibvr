#pragma once

#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include "clock.h"
#include "macros.h"
#include "internal/timer.h"
#include "irqs.h"

namespace _buttons {
  static const uint8_t block_rounds = 10;
  static const uint8_t ms_per_round = 5;
  
  enum State {
    Released = 0,
    Pressed = 1,
    // TODO MALTA NOTES: 300 should either be a global constant or a template argument
    LongReleased = (300 / ms_per_round) * 2,   // 300ms and bit0 is used as indicator for Released / Pressed
    LongPressed = (300 / ms_per_round) * 2 + 1 // 300ms and bit0 is used as indicator for Released / Pressed
  };
  
  enum Type {
    Pressed_High,
    Pressed_Low
  };
  
  enum InitIo {
    No,
    HighZ,
    Pullup
  };
  
  template <typename Pin>
  static inline decltype(PCMSK0)& _pcmsk() {
    if (Pin::port == _macros::B) {
      return PCMSK0;
    }
    if (Pin::port == _macros::C) {
      return PCMSK1;
    }
    if (Pin::port == _macros::D) {
      return PCMSK2;
    }
    static_assert(Pin::port == _macros::B || Pin::port == _macros::C || Pin::port == _macros::D, "Port invalid for retrieval of PCMSK*");
    // should never reach this
    return PCMSK0;
  }
  
  template <typename ...Pins>
  class PinList {
  };
  
  struct DoNothingMatrixListener {
    template <typename Row, typename Column>
    static void state_changed(const State& state, const uint8_t& row, const uint8_t column) {}
  };
}

template <typename... ButtonList>
class Buttons {

private:
  
  // from http://en.wikipedia.org/wiki/Variadic_Templates
  struct pass {
    template<typename ...Args> pass(Args...) {}
  };
  
  template <typename Button>
  static inline decltype(PCMSK0)& _pcmsk() {
    typedef typename Button::Pin Pin;
    return _buttons::_pcmsk<Pin>();
  }
  
  template <typename Button>
  static inline void init_button(uint8_t& pcifr, uint8_t& pcmsk0, uint8_t& pcmsk1, uint8_t& pcmsk2) {
    typedef typename Button::Pin Pin;
    if (Button::InitIo == _buttons::HighZ) {
      SET_BIT(Pin, PORT, 0); // set high z
      SET_BIT(Pin, DDR, 0);  // set to input
    } else if (Button::InitIo == _buttons::Pullup) {
      SET_BIT(Pin, DDR, 0);  // set to input
      SET_BIT(Pin, PORT, 1); // set pullup
    }
    if (Pin::port == _macros::B) {
      pcifr |= _BV(PCIF0); // enable irq for logic change on port B
    }
    if (Pin::port == _macros::C) {
      pcifr |= _BV(PCIF1); // enable irq for logic change on port C
    }
    if (Pin::port == _macros::D) {
      pcifr |= _BV(PCIF2); // enable irq for logic change on port D
    }
    _pcmsk<Button>() |= _BV(Pin::bit); // enable Button bit in pin change mask for port B
  }
  
  // every button has its own state (uint8_t)
  // bit 0 is used for button down or up state
  // all other bits are counters.
  // → 0b01010001 is button pressed
  //   0b01010000 is button up
  template <typename Button>
  static inline uint8_t& get_state() {
    static uint8_t state = _buttons::Released;
    return state;
  }
  
  template <class I, typename Button>
  static inline void handle() {
    typedef typename Button::Pin Pin;
    if (Pin::port == _macros::B && I::irq != _irqs::IRQ_PCINT0) return;
    if (Pin::port == _macros::C && I::irq != _irqs::IRQ_PCINT1) return;
    if (Pin::port == _macros::D && I::irq != _irqs::IRQ_PCINT2) return;
    
    auto& pcmsk = _pcmsk<Button>();
    if (!(pcmsk & Pin::bit)) return;  // irq for this bit is disabled

    const uint8_t p = GET_BIT(Pin, PIN);
    const uint8_t pin_now = Button::Type == _buttons::Pressed_High ? p : !p;
    uint8_t& pin_state = get_state<Button>();

    if ((pin_now && (pin_state & 1)) || (!pin_now && !(pin_state & 1))) return; // state hasn't changed
    
    _buttons::State new_state = (!pin_now) ? _buttons::Released : _buttons::Pressed;
    
    // don't act on any more irqs for some time (see run())
    pcmsk &= ~_BV(Pin::bit);

    pin_state = new_state;
    Button::state_changed(new_state);
  }
  
  template <typename Button>
  static inline void update_state() {
    typedef typename Button::Pin Pin;

    uint8_t& state = get_state<Button>();
    if (state == 0xFF || state == 0xFE) return; // don't overflow
    
    state += 2; // keep bit0 untouched!
    
    if ((state / 2) == _buttons::block_rounds) {
      auto& pcmsk = _pcmsk<Button>();
      pcmsk |= _BV(Pin::bit);
      
      // call handle, just in case if button really has changed already 
      typedef _irqs::IrqWrapper<_irqs::IRQ_PCINT0> irq_pcint0;
      if (Pin::port == _macros::B) handle<irq_pcint0, Button>();
      
      typedef _irqs::IrqWrapper<_irqs::IRQ_PCINT1> irq_pcint1;
      if (Pin::port == _macros::C) handle<irq_pcint1, Button>();
      
      typedef _irqs::IrqWrapper<_irqs::IRQ_PCINT2> irq_pcint2;
      if (Pin::port == _macros::D) handle<irq_pcint2, Button>();
    }
    if (state == _buttons::LongPressed) Button::state_changed(_buttons::LongPressed);
    if (state == _buttons::LongReleased) Button::state_changed(_buttons::LongReleased);
  }
  
public:
  
  static void init() {
    uint8_t pcifr = 0;
    uint8_t pcmsk0 = 0;
    uint8_t pcmsk1 = 0;
    uint8_t pcmsk2 = 0;
    
    // go through all Button|s and set corresponding bits in PCIFC and PCMSK
    pass{(init_button<ButtonList>(pcifr, pcmsk0, pcmsk1, pcmsk2), 1)...};
    
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
    pass{(handle<I, ButtonList>(), 1)...};
  }
  
  template<typename T>
  static inline T run(const T& clock) {
    // go through all Buttons, and ++ if pressed, and / or
    // allow irq handling again.
    pass{(update_state<ButtonList>(), 1)...};
    return ms_to_units<T>(_buttons::ms_per_round);
  }
  
  static uint8_t is_enabled() {
    return 1;
  }
};

template <typename RowList, typename ColumnList, typename Listener = _buttons::DoNothingMatrixListener>
class MatrixButtons {
  
private:
  
  // from http://en.wikipedia.org/wiki/Variadic_Templates
  struct pass {
    template<typename ...Args> pass(Args...) {}
  };
  
  template <_macros::Port PortRow, uint8_t BitRow, _macros::Port PortColumn, uint8_t BitColumn>
  static inline uint8_t& _get_state() {
    static uint8_t state = _buttons::Released;
    return state;
  }
  
  // every button has its own state (uint8_t)
  // bit 0 is used for button down or up state
  // all other bits are counters.
  // → 0b01010001 is button pressed
  //   0b01010000 is button up
  template <typename PinRow, typename PinColumn>
  static inline uint8_t& get_state() {
    return _get_state<PinRow::port, PinRow::bit, PinColumn::port, PinColumn::bit>();
  }

  // go through all Pins, and ++ if pressed
  template <typename Row, typename Column>
  static inline void update_state(const uint8_t& row, uint8_t& _column) {
    const uint8_t column = _column;
    _column++;
    uint8_t& state = get_state<Row, Column>();
    
    // check for new state
    const uint8_t pin_now = !(GET_BIT(Column, PIN));
    
    if ((state / 2) < _buttons::block_rounds) {
      state += 2;
      return;
    }
    
    const uint8_t pin_changed = (pin_now && !(state & 1)) || (!pin_now && (state & 1));
    if (!pin_changed) {
      if (state == 0xFF || state == 0xFE) return; // don't overflow
      state += 2; // keep bit0 untouched!
      
      if (state == _buttons::LongPressed) Listener::template state_changed<Row, Column>(_buttons::LongPressed, row, column);
      if (state == _buttons::LongReleased) Listener::template state_changed<Row, Column>(_buttons::LongReleased, row, column);
      return;
    }
    
    SET_BIT(PIN_8, DDR, 1);
    SET_BIT(PIN_8, PORT, pin_now);
    
    _buttons::State new_state = (!pin_now) ? _buttons::Released : _buttons::Pressed;
    
    state = new_state;
    Listener::template state_changed<Row, Column>(new_state, row, column);
  }
 
  template <typename Row, typename ...Columns>
  static inline void update_states_row(uint8_t& _row) {
    const uint8_t row = _row;
    _row++;
    
    uint8_t column = 0;
    
    SET_BIT(Row, DDR, 1);
    _NOP();
    pass{(update_state<Row, Columns>(row, column), 1)...};
    SET_BIT(Row, DDR, 0);
  }
  
  template <typename Column>
  static inline void set_ddr_column() {
    SET_BIT(Column, DDR, 0);  // input
    SET_BIT(Column, PORT, 1); // input pull up
  }
  
  template <typename Row>
  static inline void set_ddr_row() {
    SET_BIT(Row, DDR, 0);  // input
    SET_BIT(Row, PORT, 0); // input high-z
  }
  
  template <typename ...Rows, typename ...Columns>
  static inline void _init(_buttons::PinList<Rows...>, _buttons::PinList<Columns...>) {
    // set DDR for pins
    // we assume, that we are allowed to do this, because we have to change
    // output values anyways when trying different rows
    pass{(set_ddr_row<Rows>(), 1)...};
    pass{(set_ddr_column<Columns>(), 1)...};
  }
  
  template<typename ...Rows, typename ...Columns>
  static inline void _run(_buttons::PinList<Rows...>, _buttons::PinList<Columns...>) {
    uint8_t row = 0;
    pass{(update_states_row<Rows, Columns>(row), 1)...};
  }
  
public:
  
  static void init() {
    _init(RowList(), ColumnList());
  }
  
  template<typename T>
  static inline T run(const T& clock) {
    _run(RowList(), ColumnList());
    return ms_to_units<T>(_buttons::ms_per_round);
  }
  
  static uint8_t is_enabled() {
    return 1;
  }
};

#define REGISTER_BUTTONS "internal/register_buttons.h"
#define REGISTER_MBUTTONS "internal/register_mbuttons.h"
