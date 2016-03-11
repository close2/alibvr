#pragma once

#include "macros.h"
#include "pgmspace.h"
#include <avr/io.h>
#include <util/delay.h>
#include "internal/adapter.h"

// USAGE
//

#pragma push_macro("DEBUG")
#ifndef DEBUG
#  define DEBUG(message)
#endif

/* BALI notes
 *
 * - change enums to enum class
 *
 * - change template args to 1 LcdSettings class.
 * 
 * - use new port interface.
 * 
 * - make TwoLineMode an template argument.
 * 
 * - remove DEBUG
 */

namespace _lcd {
  static const uint8_t enable_bit = 5;
  static const uint8_t default_reg_select_bit = 4;

  enum Command_Data {
    Command = 0,
    Data = 1
  };
  
  enum Command {
    Reset = 0b0011,
    FourBitMode = 0b0010,
    TwoLineMode = 0b00101000,
    Clr = 0x01,
    DisplayOn = 0b00001100,
    SetDisplayAddress = 0b10000000
  };
  
  // Calculated during compile time!
  // If d7 - d4 is alligned on PORTx_3 - PORTx_0 and reg_select is on PORTx as
  // well, shift the reg select bit, so that a single assignment can be used.
  // Returns the bit position of reg_select on our data byte.
  template <typename LCD_D7, typename LCD_D6, typename LCD_D5, typename LCD_D4, typename LCD_ENABLE, typename LCD_REG_SELECT>
  constexpr inline uint8_t _reg_select_bit() {
    const uint8_t reg_sel = LCD_REG_SELECT::bit;
    if (reg_sel > 3) return reg_sel;
    else return _lcd::default_reg_select_bit;
  }
  
  // Timings from the excellent Guide from Julyan Ilett
  // google for »how to use intelligent l.c.d.s ilett«

  template <typename LCD_D7, typename LCD_D6, typename LCD_D5, typename LCD_D4, typename LCD_ENABLE, typename LCD_REG_SELECT, enum _lcd::Command_Data Command_Or_Data>
  static inline void _lcd_send_nibble(const uint8_t& data_4_bit) {
    // reg_select might be on another port in which case calculating the
    // position of the reg_bit doesn't help.  If however it is on the same port
    // and the data bits are aligned, assigning the data bits and the reg bit
    // might be possible in one PORT assignment.
    const uint8_t reg_bit = _reg_select_bit<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT>();
    const uint8_t data = data_4_bit | (Command_Or_Data << reg_bit);
    if (reg_bit == 4) set_8_bits<LCD_D7, 3, LCD_D6, 2, LCD_D5, 1, LCD_D4, 0, LCD_REG_SELECT, 4, PIN_UNUSED, -1, PIN_UNUSED, -1, PIN_UNUSED, -1>(PORTB, PORTC, PORTD, data);
    if (reg_bit == 5) set_8_bits<LCD_D7, 3, LCD_D6, 2, LCD_D5, 1, LCD_D4, 0, LCD_REG_SELECT, 5, PIN_UNUSED, -1, PIN_UNUSED, -1, PIN_UNUSED, -1>(PORTB, PORTC, PORTD, data);
    if (reg_bit == 6) set_8_bits<LCD_D7, 3, LCD_D6, 2, LCD_D5, 1, LCD_D4, 0, LCD_REG_SELECT, 6, PIN_UNUSED, -1, PIN_UNUSED, -1, PIN_UNUSED, -1>(PORTB, PORTC, PORTD, data);
    if (reg_bit == 7) set_8_bits<LCD_D7, 3, LCD_D6, 2, LCD_D5, 1, LCD_D4, 0, LCD_REG_SELECT, 7, PIN_UNUSED, -1, PIN_UNUSED, -1, PIN_UNUSED, -1>(PORTB, PORTC, PORTD, data);
    _delay_us(1); // tAS Address Setup Time (140ns min) (tAS time of REG_SELECT before ENABLE becomes 1)
    // note that we could set DATA together with ENABLE, but setting it with data_select is fine
    SET_BIT(LCD_ENABLE, PORT, 1);
    _delay_us(1); // tEH Enable High Time (450ns min) and tDS Data Set up time (200ns min) (tDS == time DATA is valid till ENABLE becomes 0)
    // tDA Data Access time (320ns max) is < 0ns because we set DATA together with REG_SELECT  (tDA == time between ENABLE 1 and DATA valid)
    SET_BIT(LCD_ENABLE, PORT, 0);
    _delay_us(1); // tAH, tDH and tEL  just to be sure
    // tAH Address hold time (10ns min)  (tAH == time SELECT is still valid after ENABLE becomes 0)
    // tDH Address hold time (20ns min)  (tDH == time DATA is still valid after ENABLE becomes 0)
    // tEL Enable low time (500ns min)
  }

  template <typename LCD_D7, typename LCD_D6, typename LCD_D5, typename LCD_D4, typename LCD_ENABLE, typename LCD_REG_SELECT, enum _lcd::Command_Data Command_Or_Data>
  static inline void _lcd_send_nibbles(const uint8_t& to_lcd) {
    _lcd_send_nibble<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT, Command_Or_Data>(to_lcd >> 4);
    _lcd_send_nibble<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT, Command_Or_Data>(to_lcd & 0x0F);
    _delay_us(40); // maximum busy time, as we don't query the BUSY line
  }

  template <typename LCD_D7, typename LCD_D6, typename LCD_D5, typename LCD_D4, typename LCD_ENABLE, typename LCD_REG_SELECT>
  static inline void lcd_send_command_nibbles(const uint8_t& to_lcd) {
    _lcd::_lcd_send_nibbles<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT, _lcd::Command>(to_lcd);
  }
  
  template <typename LCD_D7, typename LCD_D6, typename LCD_D5, typename LCD_D4, typename LCD_ENABLE, typename LCD_REG_SELECT>
  static inline void lcd_send_data_nibbles(const uint8_t& to_lcd) {
    _lcd::_lcd_send_nibbles<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT, _lcd::Data>(to_lcd);
  }

  // set data_direction on pins correctly
  template <typename LCD_D7, typename LCD_D6, typename LCD_D5, typename LCD_D4, typename LCD_ENABLE, typename LCD_REG_SELECT>
  static inline void lcd_io_init() {
      SET_BIT(LCD_ENABLE, PORT, 0);
      set_8_bits<LCD_D7, 3, LCD_D6, 2, LCD_D5, 1, LCD_D4, 0, LCD_ENABLE, 4, LCD_REG_SELECT, 5, PIN_UNUSED, -1, PIN_UNUSED, -1>(DDRB, DDRC, DDRD, 0b11111111);
  }
}

template <typename LCD_D7, typename LCD_D6, typename LCD_D5, typename LCD_D4, typename LCD_ENABLE, typename LCD_REG_SELECT>
class Lcd {
public:
  typedef Lcd<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT> _Lcd;
  
  static inline void io_init() {
    _lcd::lcd_io_init<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT>();
  }

  static inline void command(const uint8_t& command) {
    _lcd::lcd_send_command_nibbles<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT>(command);
  }

  static inline void data(const uint8_t& data) {
    _lcd::lcd_send_data_nibbles<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT>(data);
  }

  // TODO implement uint8 printing

  static void inline init() {
    io_init();
    _delay_ms(15);
    DEBUG("reset");
    _lcd::_lcd_send_nibble<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT, _lcd::Command>(_lcd::Reset);
    _delay_ms(5);
    _lcd::_lcd_send_nibble<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT, _lcd::Command>(_lcd::Reset);
    _delay_ms(1);
    _lcd::_lcd_send_nibble<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT, _lcd::Command>(_lcd::Reset);
    _delay_ms(1);
    // switch to 4bit mode:
    DEBUG("4bit");
    _lcd::_lcd_send_nibble<LCD_D7, LCD_D6, LCD_D5, LCD_D4, LCD_ENABLE, LCD_REG_SELECT, _lcd::Command>(_lcd::FourBitMode);
    _delay_us(40);
    // 2-line mode
    DEBUG("2l");
    command(_lcd::TwoLineMode);
    // clear everything
    DEBUG("clr");
    command(_lcd::Clr);
    _delay_ms(2);
    // turn on
    DEBUG("lcd On");
    command(_lcd::DisplayOn);
    _delay_ms(2);
  }

private:
  static inline void _data_char(uint8_t c) {
    data(c);
  }

public:
  
  template <typename T>
  static void print(const T& t) {
    Adapter::apply<_Lcd::_data_char>(t);
  }

  template <typename T>
  inline _Lcd& operator<<(const T& t) {
    print(t);
    return *this;
  }
  
  static void set_display_address(const uint8_t& pos) {
    command(_lcd::SetDisplayAddress | pos);
  }
};

#pragma pop_macro("DEBUG")
