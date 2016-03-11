#pragma once

#include "modbus.h"
#include "uart.h"


/* BALI notes
 * modbus.h doesn't handle the communication detail. This file handles
 * 4bit ↔ 8bit conversion and checksum verification.
 * Valid bytes are passed to Modbus class.
 * - remove all get_xxx and make those variables static
 * - change tx_start to use tx_starter
 */


/*
Code walk through with an example:
11 03 00 6B 00 01 
[0]    11 → addressId
[1]    03 → ReadHoldingRegisters
[2][3] 00 6b → 107 (modbus spec name is 40108 because for holding registers the
                    naming convention is to add 40001 to the "real" address)
[4][5] 00 01 → number of registers to read (we only support 1; see below)
which becomes:
: 1 1  0 3  0 0  6 B  0 0  0 1  8 0  CR LF
  [0]  [1]  [2]  [3]  [4]  [5]  [6]
SerialModbus starts with state = Receiving_Waiting_For_Start and crc_c = 0 and partial_char 0xFF;
rx(c) will discard everything until the first ':'
   rx(':') state ← Receiving_Address_Id
[0h]rx('1') partial_char ← 1; state stays
[0l]rx('1') last_byte ← 11; state ← Receiving_Data; crc ← 11; partial_char ← 0xFF
[1h]rx('0') partial_char ← 0; state stays
[1l]rx('1') Modbus::rx(last_byte/11); last_byte ← 01; state stays; crc ← 12; partial_char ← 0xFF
[2h]rx('0') partial_char ← 0; state stays
[2l]rx('0') Modbus::rx(last_byte/01); last_byte ← 01; state stays; crc ← 12; partial_char ← 0xFF
[3h]rx('6') partial_char ← 6; state stays
[3l]rx('B') Modbus::rx(last_byte/00); last_byte ← 01; state stays; crc ← 7d; partial_char ← 0xFF
[4h]rx('0') partial_char ← 0; state stays
[4l]rx('0') Modbus::rx(last_byte/6b); last_byte ← 01; state stays; crc ← 7d; partial_char ← 0xFF
...
[6h]rx('8') partial_char ← 8; state stays
[6l]rx('0') Modbus::rx(last_byte/01); last_byte ← 80; state stays; crc ← 00; partial_char ← 0xFF
    rx(CR)  verify crc == 0; last_byte ← CR; everything else stays
    rx(LF)  verify last_byte = CR; state ← Sending_Start; crc ← 0; partial_char ← 0xFF; Modbus::process
    
Modbus therefore receives:
reset; rx: 11, 03, 00, 6B, 00, 01; followed by process

If Modbus wants to send a response it will no longer accept character → Modbus::can_rx() returns false;
Modbus should not do long processing inside the process() function, because we are still inside
an irq handler!
When Modbus has finished building the response it will initiate the transfer by
calling SerialModbus::tx_start  (Notice that SerialModbus is a template argument
of Modbus.  SerialModbus will then enable irqs for Uart transmitting.)

For irq sending Uart calls SerialModbus::tx_is_empty() and tx_pop()
SerialModbus is a template argument of Uart!
During transmission partial_char is used for storing 4bit-values (same as during
reception) and state is used to as flag to know if crc, cr and lf have already be
sent.

SerialModbus simply wraps modubs tx, rx,... and splits/combines bytes to 4bit
representation and adds CRC, CR, LF.
*/


namespace _serial_modbus {
  enum CtrlChar {
    Start = ':', // 0x3A
    End1 = 0x0D, // CR
    End2 = 0x0A // LF
  };
  
  enum State {
    Receiving_Waiting_For_Start = 0x00,
    Receiving_Address_Id = 0x01,
    Receiving_Data = 0x02,
    Sending_Start = 0x10,
    Sending_Data = 0x11,
    Sending_End1 = 0x12,
    Sending_End2 = 0x13,
    Sending_Done = 0x14
  };
}

template <
          // Modbus template arguments:
          uint8_t addressId,
          typename ModbusDataList,
          // Uart template arguments:
          uint16_t baud,
          // uint8_t tx_buffer_size,
          // uint8_t rx_buffer_size,
          // uint8_t use_irqs,
          // class IrqTask = _uart::DoNothing, // Task to run in irq after having received a character
          _uart::StopBits stop_bits = _uart::StopBits2, // stop_bits are ignored by the receiver, you should probably always leave them at 2
          _uart::Parity parity_bit = _uart::No,
          _uart::CharacterSize data_bits = _uart::DataBits8,
          uint8_t baud_tol = BAUD_TOL>
class SerialModbus {
private:
  static inline enum _serial_modbus::State& get_state() {
    static enum _serial_modbus::State state = _serial_modbus::Receiving_Waiting_For_Start;
    return state;
  }
  
  // Modbus transmission is in 7bit mode.
  // Every data byte is transmitted in 2 4bit characters →
  // Ascii 0-9 and A-F and must be combined for the data
  // Example: the string "1FCA" must be converted to 2 databytes: 0x1F and 0xCA
  // partial_char must be < 0x10 because transmission is in 7bit
  // if it is 0xFF start a new partial_char
  static inline uint8_t& get_partial_char() {
    static uint8_t partial_char = 0;
    return partial_char;
  }
  
  static inline uint8_t& get_crc_c() {
    static uint8_t crc_c = 0;
    return crc_c;
  }
  
  static inline uint8_t& get_last_byte() {
    static uint8_t last_byte = 0x00;
    return last_byte;
  }
  
  static inline void add_crc(const uint8_t& c) {
    get_crc_c() += c;
  }
  
  static inline uint8_t _split_into_4bits(const uint8_t& c) {
    get_partial_char() = Adapter::v4bit_to_hex_char(c & 0x0F);
    return Adapter::v4bit_to_hex_char(c >> 4);
  }
 
public:
  typedef SerialModbus<addressId, ModbusDataList, baud, stop_bits, parity_bit, data_bits> _SerialModbus;
  typedef Modbus<addressId, ModbusDataList, _SerialModbus> _Modbus;
  typedef Uart<baud, 0, 0, 1, _SerialModbus, stop_bits, parity_bit, data_bits, baud_tol> _Uart;
  
  static void init() {
    _Uart::init();
  }
  
  static void reset() {
      get_state() = _serial_modbus::Receiving_Waiting_For_Start;
      get_partial_char() = 0xFF;
      get_crc_c() = 0;
      get_last_byte() = 0x00;
      _Modbus::reset();
  }
  
  static inline void rx(uint8_t c) {
    _serial_modbus::State& state = get_state();
    if ((uint8_t)state > 0x0F /* Sending */ || !_Modbus::can_rx()) {
      // response has probably not yet been build
      // ignore all characters until previous message has been processed
      return;
    }
    if (c == _serial_modbus::Start) {
      reset();
      state = _serial_modbus::Receiving_Address_Id;
      return;
    }
    if (state == _serial_modbus::Receiving_Waiting_For_Start) {
      // waiting for _serial_modbus::Start
      return;
    }
    
    uint8_t& last_byte = get_last_byte();
    uint8_t& partial_char = get_partial_char();
    
    uint8_t& crc = get_crc_c();
    if (c == _serial_modbus::End1) {
      // don't send last_byte to Modbus
      if (crc != 0) {
        reset();
        return;
      }
      last_byte = _serial_modbus::End1;
      return;
    } else if (c == _serial_modbus::End2) {
      if (last_byte != _serial_modbus::End1) {
        reset();
        return;
      }
      // prepare for reply
      crc = 0;
      partial_char = 0xFF;
      state = _serial_modbus::Sending_Start;
      // and tell Modbus to process the command
      _Modbus::process();
      return;
    } else if (last_byte == _serial_modbus::End1 || last_byte == _serial_modbus::End2) {
      reset();
      return;
    }
    
    // For every byte after the start-byte we build the value in half-bytes
    if (partial_char == 0xFF) {
      partial_char = Adapter::hex_to_v4bit(c);
      if (partial_char > 0x0F) {
        reset();
      }
      return;
    }
    
    // we have received the second 4bits of a byte.
    // send Modbus the last completely received byte (making last_byte available again).
    // unless this is the first character we have received
    if (state == _serial_modbus::Receiving_Data) {
      _Modbus::rx(last_byte);
    }

    // store byte in last_byte
    last_byte = Adapter::hex_to_v4bit(c); // first store first half
    if (last_byte > 0x0F) {
      reset();
      return;
    }
    last_byte |= partial_char << 4; // then add second half

    partial_char = 0xFF;
    add_crc(last_byte);
    
    // shortcut to stop receiving if it is another addressId:
    if (state == _serial_modbus::Receiving_Address_Id) {
      if (last_byte != 0 && last_byte != addressId) {
        reset();
        return;
      }
      state = _serial_modbus::Receiving_Data;
    }
  }
  
  
  // Modbus does not give us crc, cr and lf and does not split data bytes
  // into 4bit values.
  static uint8_t tx_is_empty() {
    const uint8_t empty = _Modbus::tx_is_empty();
    _serial_modbus::State& state = get_state();
    
    if (state == _serial_modbus::Sending_Start && empty) {
      // Nothing to send, prepare for Receiving
      reset();
      return 1;
    }
    // Sending_Start → before sending :
    // Sending_Data  → before or during sending data from Modbus
    //                 and sending crc
    // Sending_End1  → before sending End1 (CR)
    // Sending_End2  → before sending End2 (LF)
    // Sending_Done  → Done
    if (state == _serial_modbus::Sending_Done) {
      // finished Sending, prepare for Receiving
      reset();
      return 1;
    }
    return 0;
  }
  
  static uint8_t tx_pop() {
    _serial_modbus::State& state = get_state();
    if (state == _serial_modbus::Sending_Start) {
      state = _serial_modbus::Sending_Data;
      return ':';
    }

    uint8_t& partial_char = get_partial_char();
    if (partial_char != 0xFF) {
      uint8_t ret = partial_char;
      partial_char = 0xFF;
      return ret;
    }

    switch (state) {
      case _serial_modbus::Sending_End1:
        state = _serial_modbus::Sending_End2;
        return _serial_modbus::End1;
      case _serial_modbus::Sending_End2:
        state = _serial_modbus::Sending_Done;
        return _serial_modbus::End2;
      default:
        if (_Modbus::tx_is_empty()) {
          state = _serial_modbus::Sending_End1; // switch to Sending_End1 (CR)
          const uint8_t& crc = get_crc_c();     // but send CRC first.
          return _split_into_4bits(-crc);
        }
        const uint8_t& c = _Modbus::tx_pop();
        add_crc(c);
        return _split_into_4bits(c);
    }
  }
  
  static inline void tx_start() {
    // we must already be in Sending mode
    _Uart::tx_irq();
  }
  
  static void tx_starter(void (*f)()) {
    // don't do anything, we start transmission by calling tx_irq directly
  }
  
  static void tx_done() {
    // nothing to do
  }
};

#define REGISTER_SERIAL_MODBUS "internal/register_serial_modbus.h"
