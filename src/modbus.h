#pragma once

#include <stdio.h>
#include "internal/adapter.h"
#include "internal/task_list.h"

/* BALI Notes
 * - explain template argument list; add some example implementations in other
 *   languages (sh, dart, C, python)
 * - Explain ConnectionCtrl template argument (::reset(), ::tx_start())
 * - improve doc: Modubs is a task, which enables itself as soon as a complete
 *   message is available.
 * - In _exec1ModbusData return values seem inversed (1 instead of 0 and ↔)
 */

// Handling reading or writing multiple values is complicated.
// We simply don't support it, and return IllegalDataAddress
// modbusData functions should for instance only be mapped onto even addresses
// in which case this would be the correct response
// (Note however, that this code always returns IllegalDataAddress if more
// than one address should be written to or read from, even if modbusData
// functions for those addresses are registered.)

/*
 * Difference between coils and discrete inputs:
 * 
 * copied from: http://www.ni.com/white-paper/7675/en/
 * 
 * The Discrete Inputs represent a single bit (Boolean) which can only be read.
 * In other words, the master can only perform a read action on the discrete inputs.
 * The same holds for the Input Registers.
 * The master can only read the slave’s Input Registers.
 * The difference between the Discrete Inputs and the Input Registers is that
 * the Input Registers represent 16 bits while the Discrete Inputs are only a
 * single bit.
 * The Coils also represent a Boolean data type which can be read and written to by the master.
 * The Holding Registers represent a 16 bit word that can be read and written to.
 *
 * +-------+----------------+-------------+
 * |       |      read      |     r/w     |
 * +-------+----------------+-------------+
 * | 1bit  | discrete input | coil        |
 * | 16bit | input reg      | holding reg |
 * +-------+----------------+-------------+
 *
 */

namespace _modbus {
  
  enum Function {
    ReadDiscreteInputs = 0x02,
    ReadCoils = 0x01,
    WriteSingleCoil = 0x05,
    // WriteMultipleCoils = 0x15, // Not supported
    // 16bit functions:
    ReadInputRegisters = 0x04,
    ReadHoldingRegisters = 0x03,
    WriteSingleRegister = 0x06,
    // WriteMultipleRegisters = 0x16,
  };
  
  enum ReadWrite {
    Read,
    Write
  };
  
  enum DataType {
    DiscreteInput = 0x00,
    Coil = 0x01,
    InputRegister = 0x02,
    Register = 0x03
  };
  
  enum Exception {
    IllegalFunction = 0x01,
    IllegalDataAddress = 0x02
  };
  
  
  template <enum ReadWrite RW, template <typename ...> class List, typename T, typename ...Tasks>
  static uint8_t execModbusData(uint16_t& value, const enum DataType& dataType, const uint16_t& address);
  
  template <enum ReadWrite RW, typename T>
  inline static uint8_t _exec1ModbusData(uint16_t& value, const enum DataType& dataType, const uint16_t& address) {
    if (RW == _modbus::Read) {
      if (T::is_get(dataType, address)) {
        value = T::get(dataType, address);
        return 0;
      }
    } else {
      if (T::is_set(dataType, address)) {
        T::set(dataType, address, value);
        return 0;
      }
    }
    return 1;
  }
  
  template <enum ReadWrite RW, typename T>
  inline static uint8_t execModbusData(uint16_t& value, const enum DataType& dataType, const uint16_t& address, _task_list::TaskList<T>) {
    return _exec1ModbusData<RW, T>(value, dataType, address);
  }

  template <enum ReadWrite RW, typename T, typename ...Tasks>
  inline static uint8_t execModbusData(uint16_t& value, const enum DataType& dataType, const uint16_t& address, _task_list::TaskList<T, Tasks...>) {
    if (_exec1ModbusData<RW, T>(value, dataType, address)) {
      return 1;
    } else {
      return execModbusData<RW>(value, dataType, address, _task_list::TaskList<Tasks...>());
    }
  }
}

template <uint8_t addressId, typename ModbusDataList, typename ConnectionCtrl>
class Modbus {
private:
  static const uint8_t _m_size = 7;
  
  enum Mode {
    Receiving,
    Processing,
    Sending
  };
  
  static Mode& get_mode() {
    static Mode mode = Receiving;
    return mode;
  }
  
  // input and/or output message
  // during Sending mode message[0] which is either addressId or 0 for broadcast
  // is used as message-length indicator.
  static inline uint8_t* get_message() {
    static uint8_t message[_m_size];
    return message;
  }
  
  // 0xFF for not started yet
  static inline uint8_t& get_pos() {
    static uint8_t pos = 0xFF;
    return pos;
  }
  
  static inline void _restart() {
    get_pos() = 0;
    get_mode() = Receiving;
  }
  
  static inline void _error() {
    ConnectionCtrl::reset();
    reset(); // yes reset() is called twice, because ConnectionCtrl will
    // probably also call reset(), but compiler should optimize this.
  }
  
  static void _exception(_modbus::Exception ex) {
    uint8_t* message = get_message();
    // only send error message if this was sent directly to us
    if (message[0] == 0) {
      _error();
      return;
    }
    // first add 0x80 to function code:
    message[1] += 0x80;
    // then set the exception code:
    message[2] = ex;
    
    // set the size:
    message[0] = 3;
    
    // and send message:
    _send_reply();
  }
  
  static void _send_reply() {
    get_pos() = 0;
    get_mode() = Sending;
    ConnectionCtrl::tx_start();
  }
  
public:
  static inline void reset() {
    _restart();
  }
  
  static uint8_t can_rx() {
    return get_mode() == Receiving;
  }
  
  static void rx(uint8_t c) {
    uint8_t* message = get_message();
    uint8_t& pos = get_pos();
    const Mode& mode = get_mode();
    
    if (mode != Receiving) {
      return;
    }
    
    // restart if message buffer is too small
    if (pos >= _m_size) {
      _error();
      return;
    }
    
    message[pos++] = c;
  }
  
  static uint8_t tx_is_empty() {
    Mode& mode = get_mode();
    if (mode == Sending && get_pos() >= get_message()[0]) {
      _restart();
    }
    return mode != Sending;
  }
  
  static uint8_t tx_pop() {
    uint8_t* message = get_message();
    uint8_t& pos = get_pos();
    if (pos == 0) {
      pos++;
      return addressId;
    }

    return message[pos++];
  }
  
  static void process() {
    Mode& mode = get_mode();
    mode = Processing;
  }
  
  // ===v===v===v=== TASK interface ===v===v===v===
  static uint8_t is_enabled() {
    return get_mode() == Processing;
  }
  
  template <typename T>
  static T run(const T& clock) {
    const uint8_t pos = get_pos();
    uint8_t* message = get_message();
    
    // for now allow messages > than 6 even though we don't support them
    if (pos < 6) {
      _error();
    }

    // message[0] is our addressId or 0
    const uint8_t functionCode = message[1];
    
    const uint16_t address = message[2] << 8 | message[3];
    _modbus::ReadWrite readWrite;
    _modbus::DataType type;

    // value or count of read values
    const uint16_t value = message[4] << 8 | message[5];
    
    uint8_t bitOperation;

    switch(functionCode) {
      case _modbus::ReadDiscreteInputs:
        readWrite = _modbus::Read;
        type = _modbus::DiscreteInput;
        bitOperation = 1;
        break;
      case _modbus::ReadCoils:
        readWrite = _modbus::Read;
        type = _modbus::Coil;
        bitOperation = 1;
        break;
      case _modbus::WriteSingleCoil:
        readWrite = _modbus::Write;
        type = _modbus::Coil;
        bitOperation = 1;
        break;
      case _modbus::ReadInputRegisters:
        readWrite = _modbus::Read;
        type = _modbus::InputRegister;
        bitOperation = 0;
        break;
      case _modbus::ReadHoldingRegisters:
        readWrite = _modbus::Read;
        type = _modbus::Register;
        bitOperation = 0;
        break;
      case _modbus::WriteSingleRegister:
        readWrite = _modbus::Write;
        type = _modbus::Register;
        bitOperation = 0;
        break;
      default:
        _exception(_modbus::IllegalFunction);
        return 0;
    }
    
    // we do not support multiple read operations
    if (readWrite == _modbus::Read && value != 1) {
      _exception(_modbus::IllegalDataAddress);
      return 0;
    }
    
    // only message with length 6 are possible, otherwise we received an invalid frame
    if (pos != 6) {
      _error();
    }
    
    if (readWrite == _modbus::Write) {
      uint16_t v = value;
      if (!_modbus::execModbusData<_modbus::Write>(v, type, address, ModbusDataList())) {
        _exception(_modbus::IllegalDataAddress);
        return 0;
      }
      if (message[0] == 0) {
        // broadcast
        _restart();
        return 0;
      }
      // echo the message back to sender
      message[0] = pos;
    } else {
      uint16_t v = 0;
      if (!_modbus::execModbusData<_modbus::Read>(v, type, address, ModbusDataList())) {
        _exception(_modbus::IllegalDataAddress);
        return 0;
      }
      
      if (message[0] == 0) {
        // broadcast
        _restart();
        return 0;
      }
      if (bitOperation) {
        v = (v != 0) ? 1 : 0;
        message[2] = 1;
        message[3] = v;
        message[0] = 4;
      } else {
        message[2] = 2;
        message[3] = v >> 8;
        message[4] = v & 0x00FF;
        message[0] = 5;
      }
    }
    _send_reply();
    
    // we wan't to be called right away if we are enabled. (which we are, when a
    // complete message has been received)
    return 0;
  }
};


// TODO MALTA NOTES
// instead of Macro create template class.
// pass function to template

#define MODBUS_DATA(name, type, addr) \
class name { \
private: \
  static inline uint8_t _is_set_or_get(const _modbus::DataType& dataType, const uint16_t& address) { \
    return dataType == type && addr == address; \
  } \
  static inline uint16_t _set_or_get(const _modbus::ReadWrite& read_or_write, const _modbus::DataType& dataType, const uint16_t& address, const uint16_t& value); \
public: \
  static inline void set(const _modbus::DataType& dataType, const uint16_t& address, const uint16_t& v) { \
    _set_or_get(_modbus::Write, dataType, address, v); \
  } \
  static inline uint16_t get(const _modbus::DataType& dataType, const uint16_t& address) { \
    return _set_or_get(_modbus::Read, dataType, address, 0); \
  } \
  static inline uint8_t is_get(const _modbus::DataType& dataType, const uint16_t& address) { \
    return _is_set_or_get(dataType, address); \
  } \
  static inline uint8_t is_set(const _modbus::DataType& dataType, const uint16_t& address) { \
    return _is_set_or_get(dataType, address); \
  } \
}; \
uint16_t name::_set_or_get(const _modbus::ReadWrite& read_or_write, const _modbus::DataType& dataType, const uint16_t& address, const uint16_t& value)

#define MODBUS_DATA_LIST _task_list::TaskListEmpty
#define REGISTER_MODBUS_DATA "internal/register_modbus_data.h"
