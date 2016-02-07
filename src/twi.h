#pragma once

#include "macros.h"
#include "buffer.h"
//#include "irqs.h"
//#include "tasks.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
//#include "pgmspace.h"
#include "internal/adapter.h"
#include "clock_select.h"
#include "transmission.h"

#include "type_traits.h"


// FIXME we will change datadirection changes to be blocking until
// buffer is empty.
// Non blocking behaviour can only be done by using the IRQ handler.

namespace _twi {
  
  const uint32_t FAST_MODE_SPEED = 400000;
  
  typedef _transmission::DataDirection DataDirection;
  
  typedef _transmission::ErrorCheck ErrorCheck;
  
  typedef _transmission::ToBuffer ToBuffer;
  
  typedef _transmission::ClearErrors ClearErrors;
  
  struct _BitRateConfig {
    uint8_t twsr; // TWPS1 and TWPS0 (i.e. the prescaler)
    uint8_t twbr;
  };
  
  constexpr _BitRateConfig calculateBitRateConfig(uint32_t max_speed) {
    // twps == 0 ↔ prescaler == 1
    // twps == 1 ↔ prescaler == 4
    // twps == 2 ↔ prescaler == 16
    // twps == 3 ↔ prescaler == 64
    
    // SCL_frequency = F_CPU / (16 + 2 * TWBR * prescaler)
    
    // Prescaler should be as small as possible so that the precission is
    // as high as possible.
    _BitRateConfig result;
    
    const uint32_t lowestSpeedPs0 = F_CPU / (16 + 2 * 255);
    const uint32_t lowestSpeedPs1 = F_CPU / (16 + 2 * 255 * 4);
    const uint32_t lowestSpeedPs2 = F_CPU / (16 + 2 * 255 * 16);
    const uint32_t lowestSpeedPs3 = F_CPU / (16 + 2 * 255 * 64);
    
    // SCL_frequency * (16 + 2 * TWBR * prescaler) = F_CPU
    // 16 * SCL_frequency + 2 * TWBR * prescaler * SCL_frequency = F_CPU
    // TWBR = (F_CPU - 16 * SCL_frequency) / (2 * prescaler * SCL_frequency)
    
    uint8_t prescaler;
    if (lowestSpeedPs0 <= max_speed) {
      result.twsr = 0;
      prescaler = 1;
    } else if (lowestSpeedPs1 <= max_speed) {
      result.twsr = _BV(TWPS0);
      prescaler = 4;
    } else if (lowestSpeedPs2 <= max_speed) {
      result.twsr = _BV(TWPS1);
      prescaler = 16;
    } else if (lowestSpeedPs3 <= max_speed) {
      result.twsr = _BV(TWPS0);
      result.twsr = _BV(TWPS1);
      prescaler = 64;
    } else {
      // we can't even go so slow.
      result.twsr = _BV(TWPS0);
      result.twsr = _BV(TWPS1);
      result.twbr = 255;
      return result;
    }
    
    result.twbr = (F_CPU - 16 * max_speed) / (2 * prescaler * max_speed);
    if (F_CPU / (16 + 2 * result.twbr * prescaler) > max_speed) {
      // rounding error from division.
      result.twbr--;
    }
    return result;
  }
  

  // TODO combine uart and spi and twi DoNothing and move outside
  class DoNothing {
  public:
    template <typename T>
    static ToBuffer rx_complete(uint8_t c) {
      return ToBuffer::Yes;
    }
    
    template <typename T>
    static void tx_complete() {
    }
    
    template <typename T>
    static void tx_empty() {
    }
  };
  
  
  template <uint8_t address,
            uint32_t maxSpeed = _twi::FAST_MODE_SPEED,
            int8_t useIrqs = 0,
            int8_t generalCall = 0,
            uint8_t addressMask = 0x00>
  static void _init() {
    uint8_t twbr = 0;  // bit rate reg
    uint8_t twsr = 0;  // status reg
    uint8_t twcr = 0;  // control reg
    uint8_t twar = 0;  // address reg
    uint8_t twamr = 0; // address mask reg
    
    twar = address << 1 | (generalCall ? 1 : 0);
    twamr = addressMask << 1;
    
    
    twcr |= _BV(TWEN);
    if (useIrqs) twcr |= _BV(TWIE);
    
    const _BitRateConfig config = calculateBitRateConfig(maxSpeed);
    
    twbr = config.twbr;
    twsr = config.twsr;
    
    TWBR = twbr;
    TWSR = twsr;
    TWCR = twcr;
    TWAR = twar;
    TWAMR = twamr;
  }
  
}



template <_spi::ClockSelect ClockSelect,
          _spi::Mode Mode,
          typename _SS, // in slave mode ignored
          _spi::DataOrder DataOrder = _spi::DataOrder::MSB,
          _spi::DataMode DataMode = _spi::DataMode::Mode0>
class SpiSync {
  
private:
  typedef SpiSync<ClockSelect, Mode, _SS, DataOrder, DataMode> _Spi;
  
  // force SS to SS-pin (PIN_10) if in slave mode
  typedef typename std::conditional<Mode == _spi::Mode::Slave, PIN_10, _SS>::type SS;

  // non blocking version.  Possibly produces garbage if can_t_rx is false
  static void _tx(uint8_t c) {
    UDR0 = c;
  }

public:
  static void init() {
    _spi::_init<Mode, DataOrder, DataMode, 0, ClockSelect>();
  }
  
  // this works only because we don't use irqs (irq handler clears this flag)
  inline static uint8_t can_tx_rx() {
    // TODO if Master and SS is not in data transfer mode we can.
    return SPSR & _BV(SPIF); // transmission complete
  }

  static uint8_t tx_rx(uint8_t c) {
    while (!can_tx_rx()); // loop until bit is set
    _tx(c);
    while (!can_tx_rx()); // loop until bit is set
    return rx();
  }
  
  // non blocking version.  Reads the current buffer.
  static uint8_t rx() {
     return UDR0;
  }

  template <typename T>
  static void tx(const T& t) {
    Adapter::apply<_Spi::_tx>(t);
  }
  
  inline _Spi& operator>>(uint8_t& c) {
    tx_rx(c);
    return *this;
  }

  template <typename T>
  inline _Spi& operator<<(const T& t) {
    tx_rx(t);
    return *this;
  }
};

/*
template <uint8_t tx_buffer_size,
          uint8_t rx_buffer_size,
          class IrqTask = _spi::DoNothing, // Task to run in irq after having received a character
          uint8_t default_char = 0xFF, // Send this in READ mode
          _spi::DataOrder DataOrder = _spi::DataOrder::MSB,
          _spi::DataMode DataMode = _spi::DataMode::Mode0>
class SpiSlave2 {
private:
  
  typedef SpiSlave2<tx_buffer_size, rx_buffer_size, IrqTask, default_char, DataOrder, DataMode> _SpiSlave;
  
  static uint8_t& _getStatus() {
    // The lowest 4 bits are reserved for datadirection.  If msb is set, we had an error.
    // 5th bit: 1 → sending; 0 → need to start transmission
    static uint8_t status = 0;
    return status;
  }
  
  static uint8_t _transmitting() {
    return _getStatus() & _BV(5);
  }
  
  static void _transmitting(uint8_t v) {
    if (v) _getStatus() |= _BV(5);
    else _getStatus() &= ~(_BV(5));
  }
  
  class Spi_Tx_Buffer_Id {};
  typedef Static_Buffer<tx_buffer_size, Spi_Tx_Buffer_Id, _buffer::Marker::WithMarkers> tx_buffer;
  class Spi_Rx_Buffer_Id {};
  typedef Static_Buffer<rx_buffer_size, Spi_Rx_Buffer_Id> rx_buffer;

  static void _setDD(_spi::DataDirection dd) {
    _getStatus() = (_getStatus() & ~0x0F) | (uint8_t) dd;
  }
  
  static uint8_t _getDD() {
    return _getStatus() & 0x0F;
  }
  
  static uint8_t __tx(uint8_t c, uint8_t marked) {
    if (marked) {
      if (c == (uint8_t) _spi::DataDirection::EndFrame) {
        _ss(0);
      }
      // change data direction
      _setDD((_spi::DataDirection) c);
      return 1;
    } else {
      _ss(1);
      _transmitting(1);
      SPDR = c;
    }
    return 0;
  }
  
  // also used from inside irq handler
  static uint8_t __tx() {
    uint8_t marked;
    auto c = tx_buffer::pop(marked);
    return __tx(c, marked);
  }

  static void _tx(uint8_t c, uint8_t mark) {
    if (tx_buffer_size == 0) __tx(c, mark);
    else {
      while(tx_buffer::is_full()) {
        // wait till one character has been sent
      }
      
      if (Mode == _spi::Mode::Master && !_transmitting() && !tx_buffer::is_empty()) {
        __tx(c, mark);
      } else {
        tx_buffer::push(c, mark);
      }
    }
  }
  
  static void _tx(uint8_t c) {
    _tx(c, 0);
  }

  static uint8_t _rx(uint8_t& marked) {
    return rx_buffer::pop(marked);
  }

  static uint8_t _rx() {
    uint8_t dummy;
    return _rx(dummy);
  }
  
  static void _ss(uint8_t v) {
    if (std::is_same<SS, _spi::NoSS>() || Mode == _spi::Mode::Slave) {
      // don't do anything
    } else {
      if (v) SET_BIT(SS, PORT, 1);
      else SET_BIT(SS, PORT, 0);
    }
  }
  

public:
  
  // don't allow setting the marker bit in public API
  static void tx(uint8_t c) {
    _tx(c);
  }
  
  // don't allow retrieval of marker bit in public API
  static uint8_t rx() {
    return _rx();
  }

  // only works if rx_buffer_size > 0
  static uint8_t can_rx() {
    return !rx_buffer::is_empty();
  }
  
  // only works if tx_buffer_size > 0
  static uint8_t can_tx() {
    return !tx_buffer::is_full();
  }

  static void init(_spi::DataDirection initialDd = _spi::DataDirection::ReadWrite,
                   uint8_t initialValue = 0) {
    cli();
    
    if (Mode == _spi::Mode::Master) SET_BIT(_SS, DDR, 1);
    else SET_BIT(_SS, DDR, 0);
    
    _spi::_init<ClockSelect, Mode, DataOrder, DataMode, 1>();
    _setDD(initialDd);
    if (Mode == _spi::Mode::Slave) SPDR = initialValue;
    sei();
  }
  
  static void reset(_spi::DataDirection newDd = _spi::DataDirection::ReadWrite) {
    // clear all buffers and set data direction
    tx_buffer::clear();
    _getStatus() = 0;
    _setDD(newDd);
  }
  
  static void flush() {
    while (!tx_buffer::is_empty());
  }
  
  static uint8_t has_error(_spi::ErrorCheck errorCheck = _transmission::ErrorCheck::Now) {
    return 0;
    //if (errorCheck == _transmission::ErrorCheck::FlushFirst) flush();
    //return _getStatus() & 0b10000000;
  }
  
  // implement IRQ-handler interface
  // this handle function handles 1 irq SPI_STC
  template <class I>
  static inline void handle(I) {
    const uint8_t _dd = _getDD();
    
    // real dd might change in callbacks → use cached value (_dd)
    
    if (_dd == (uint8_t) _spi::DataDirection::Read ||
        _dd == (uint8_t) _spi::DataDirection::ReadOnly ||
        _dd == (uint8_t) _spi::DataDirection::ReadWrite) {
      uint8_t received = SPDR;
      const auto toBuffer = IrqTask::template rx_complete<_SpiSlave>(received);
      if (toBuffer == _spi::ToBuffer::Yes && !rx_buffer::is_full()) rx_buffer::push(received);
    } 
    if (_dd == (uint8_t) _spi::DataDirection::Write ||
        _dd == (uint8_t) _spi::DataDirection::WriteOnly ||
        _dd == (uint8_t) _spi::DataDirection::ReadWrite) {
      IrqTask::template tx_complete<_SpiSlave>();
    } 
    
    // __tx() will reset _transmitting to 1 if we send something.
    _transmitting(0);
    // loop for marked bytes  (note the break at the end → only possibility to stay in loop is continue)
    for (;;) {
      // dd might now be different to _dd (because of IrqTask or marked byte in loop)
      const uint8_t dd = _getDD();
      if (dd == (uint8_t) _spi::DataDirection::Write ||
          dd == (uint8_t) _spi::DataDirection::WriteOnly ||
          dd == (uint8_t) _spi::DataDirection::ReadWrite) {
        if (tx_buffer::is_empty()) IrqTask::template tx_empty<_SpiSlave>();
        if (!tx_buffer::is_empty()) {
          if (__tx()) continue;
        }
      }
      break;
    }
  }
  
  static void sleep_until_data_available() {
    // FIXME
    if (!_transmitting()) SPDR = '\0';
    _transmission::sleep_until_data_available<rx_buffer>();
  }
  
  static void rx(uint8_t& c) {
    if (!can_rx()) sleep_until_data_available();
    c = _rx();
  }
  
  template <typename T>
  static void tx(const T& t) {
    Adapter::apply<_SpiSlave::_tx>(t);
  }
  
  static void tx(_spi::DataDirection dd) {
    _tx((uint8_t) dd, 1);
  }
  
  static void tx(_spi::ClearErrors e) {
    // don't do anything.
  }
  
  inline _SpiSlave& operator>>(uint8_t& c) {
    rx(c);
    return *this;
  }

  template <typename T>
  inline _SpiSlave& operator<<(const T& t) {
    tx(t);
    return *this;
  }
};
*/

template <uint8_t tx_buffer_size,
          uint8_t rx_buffer_size,
          class IrqTask = _spi::DoNothing, // Task to run in irq after having received a character
          uint8_t address = 0xFF,
          uint8_t generalCall = 0,
          uint32_t maxSpeed = _twi::FAST_MODE_SPEED,
          uint8_t pull_up = 1>
class TwiMaster {
private:
  
  typedef TwiMaster<tx_buffer_size, rx_buffer_size, IrqTask, address, generalCall, maxSpeed, pull_up> _TwiMaster;
  
  class Twi_Tx_Buffer_Id {};
  typedef Static_Buffer<tx_buffer_size, Twi_Tx_Buffer_Id, _buffer::Marker::WithMarkers> tx_buffer;
  class Twi_Rx_Buffer_Id {};
  typedef Static_Buffer<rx_buffer_size, Twi_Rx_Buffer_Id> rx_buffer;
  

  static uint8_t& _getStatus() {
    // The lowest 4 bits are reserved for datadirection.  If msb is set, we had an error.
    // bit[4]: 1 → sending; 0 → need to start transmission (no byte currently sent)
    // bit[5]: cache for SS
    static uint8_t status = 0;
    return status;
  }
  static const uint8_t transmitting_bit = 4;
  static const uint8_t ss_cache_bit = 5;
  
  
  static uint8_t _transmitting() {
    return _getStatus() & _BV(transmitting_bit);
  }
  
  static void _transmitting(uint8_t v) {
    if (v) _getStatus() |= _BV(transmitting_bit);
    else _getStatus() &= ~(_BV(transmitting_bit));
  }
  
  
  static void _setDD(_twi::DataDirection dd) {
    if (dd == _spi::DataDirection::EndFrame) {
      _ss(0);
    }
    
    uint8_t& status = _getStatus();
    status = (status & 0xF0) | (uint8_t) dd;
  }
  
  static _twi::DataDirection _getDD() {
    uint8_t& status = _getStatus();
    return (_twi::DataDirection) (status & 0x0F);
  }
  
  
  // real transmitting
  static void __tx(uint8_t c, uint8_t marked) {
    if (marked) {
      // change data direction
      _setDD((_twi::DataDirection) c);
    } else {
      _ss(1);
      _transmitting(1);
      SPDR = c;
    }
  }
  
  // putting into buffer or calling __tx → the real transmitting function
  static void _tx(uint8_t c, uint8_t mark) {
    // FIXME wait for transmission of current char to end? && !_transmitting?
    if (tx_buffer_size == 0) __tx(c, mark);
    else {
      while(tx_buffer::is_full()) {
        // wait till one character has been sent
      }
      
      cli();
      if (!_transmitting() && tx_buffer::is_empty()) {
        sei();
        __tx(c, mark);
      } else {
        tx_buffer::push(c, mark);
        sei();
      }
    }
  }
  
  static void _tx(uint8_t c) {
    _tx(c, 0);
  }

  static uint8_t _rx(uint8_t& marked) {
    return rx_buffer::pop(marked);
  }

  static uint8_t _rx() {
    uint8_t dummy;
    return _rx(dummy);
  }
  

public:
  
  // don't allow setting the marker bit in public API
  static void tx(uint8_t c) {
    _tx(c);
  }
  
  // don't allow retrieval of marker bit in public API
  static uint8_t rx() {
    return _rx();
  }

  // only works if rx_buffer_size > 0
  static uint8_t can_rx() {
    return !rx_buffer::is_empty();
  }
  
  // only works if tx_buffer_size > 0
  static uint8_t can_tx() {
    return !tx_buffer::is_full();
  }

  static void init(_twi::DataDirection initialDd = _twi::DataDirection::ReadWrite,
                   uint8_t initialValue = 0) {
    cli();
    
    SET_BIT(PIN_11, DDR, 1); // MOSI
    SET_BIT(PIN_13, DDR, 1); // SCK
    
    _twi::_init<_twi::Mode::Master, max_speed, 1, ClockSelect>();
    _setDD(initialDd);
    sei();
  }
  
  static void reset(_spi::DataDirection newDd = _spi::DataDirection::ReadWrite) {
    // clear all buffers and set data direction
    tx_buffer::clear();
    _getStatus() = 0;
    _setDD(newDd);
  }
  
  static void flush() {
    while (!tx_buffer::is_empty());
  }
  
  static uint8_t has_error(_spi::ErrorCheck errorCheck = _transmission::ErrorCheck::Now) {
    if (errorCheck == _transmission::ErrorCheck::FlushFirst) flush();
    return 0;
    //return _getStatus() & 0b10000000;
  }
  
  // implement IRQ-handler interface
  // this handle function handles 1 irq SPI_STC
  template <class I>
  static inline void handle(I) {
    const auto _dd = _getDD();
    
    // real dd might change in callbacks → use cached value (_dd)
    
    if (_dd == _spi::DataDirection::Read ||
        _dd == _spi::DataDirection::ReadOnly ||
        _dd == _spi::DataDirection::ReadWrite) {
      uint8_t received = SPDR;
      const auto toBuffer = IrqTask::template rx_complete<_SpiMaster>(received);
      if (toBuffer == _spi::ToBuffer::Yes && !rx_buffer::is_full()) rx_buffer::push(received);
    } 
    if (_dd == _spi::DataDirection::Write ||
        _dd == _spi::DataDirection::WriteOnly ||
        _dd == _spi::DataDirection::ReadWrite) {
      IrqTask::template tx_complete<_SpiMaster>();
    } 
    
    // __tx() will reset _transmitting to 1 if we send something.
    _transmitting(0);
    // loop for marked bytes  (note the break at the end → only possibility to stay in loop is continue)
    for (;;) {
      // dd might now be different to _dd (because of IrqTask or marked byte in loop)
      const auto dd = _getDD();
      if (dd == _spi::DataDirection::Write ||
          dd == _spi::DataDirection::WriteOnly ||
          dd == _spi::DataDirection::ReadWrite) {
        if (tx_buffer::is_empty()) IrqTask::template tx_empty<_SpiMaster>();
        
        // buffer might be filled by IrqTask → retry
        if (!tx_buffer::is_empty()) {
          uint8_t marked;
          auto c = tx_buffer::pop(marked);
          __tx(c, marked);
          if (marked) continue;
        }
      }
      break;
    }
  }
  
  static void sleep_until_data_available() {
    _transmission::sleep_until_data_available<rx_buffer>();
  }
  
  static void rx(uint8_t& c) {
    if (!can_rx()) sleep_until_data_available();
    c = _rx();
  }
  
  template <typename T>
  static void tx(const T& t) {
    Adapter::apply<_TwiMaster::_tx>(t);
  }
  
  static void tx(_spi::DataDirection dd) {
    _tx((uint8_t) dd, 1);
  }
  
  static void tx(_spi::ClearErrors e) {
    // don't do anything.
  }
  
  inline _TwiMaster& operator>>(uint8_t& c) {
    rx(c);
    return *this;
  }

  template <typename T>
  inline _TwiMaster& operator<<(const T& t) {
    tx(t);
    return *this;
  }
};


template <uint8_t tx_buffer_size,
          uint8_t rx_buffer_size,
          class IrqTask = _spi::DoNothing, // Task to run in irq after having received a character
          uint8_t default_char = 0xFF,
          _spi::DataOrder DataOrder = _spi::DataOrder::MSB,
          _spi::DataMode DataMode = _spi::DataMode::Mode0>
class SpiSlave {
private:
  
  typedef SpiSlave<tx_buffer_size, rx_buffer_size, IrqTask, default_char, DataOrder, DataMode> _SpiSlave;
  
  class Spi_Tx_Buffer_Id {};
  typedef Static_Buffer<tx_buffer_size, Spi_Tx_Buffer_Id, _buffer::Marker::WithMarkers> tx_buffer;
  class Spi_Rx_Buffer_Id {};
  typedef Static_Buffer<rx_buffer_size, Spi_Rx_Buffer_Id> rx_buffer;
  
  typedef PIN_10 SS;
  

  static uint8_t& _getStatus() {
    // The lowest 4 bits are reserved for datadirection.  If msb is set, we had an error.
    static uint8_t status = 0;
    return status;
  }

  static void _setDD(_spi::DataDirection dd) {
    uint8_t& status = _getStatus();
    status = (status & 0xF0) | (uint8_t) dd;
  }
  
  static _spi::DataDirection _getDD() {
    uint8_t& status = _getStatus();
    return (_spi::DataDirection) (status & 0x0F);
  }
  
  
  // real transmitting
  static void __tx(uint8_t c, uint8_t marked) {
    if (marked) {
      // change data direction
      _setDD((_spi::DataDirection) c);
    } else {
      SPDR = c;
    }
  }
  
  // putting into buffer
  static void _tx(uint8_t c, uint8_t mark) {
    if (tx_buffer_size == 0) {
      __tx(c, mark);
      return;
    }
    
    while(tx_buffer::is_full()) {
      // wait till one character has been sent
    }
    tx_buffer::push(c, mark);
  }
  
  static void _tx(uint8_t c) {
    _tx(c, 0);
  }

  static uint8_t _rx(uint8_t& marked) {
    return rx_buffer::pop(marked);
  }

  static uint8_t _rx() {
    uint8_t dummy;
    return _rx(dummy);
  }
  

public:
  
  // don't allow setting the marker bit in public API
  static void tx(uint8_t c) {
    _tx(c);
  }
  
  // don't allow retrieval of marker bit in public API
  static uint8_t rx() {
    return _rx();
  }

  // only works if rx_buffer_size > 0
  static uint8_t can_rx() {
    return !rx_buffer::is_empty();
  }
  
  // only works if tx_buffer_size > 0
  static uint8_t can_tx() {
    return !tx_buffer::is_full();
  }

  static void init(_spi::DataDirection initialDd = _spi::DataDirection::ReadWrite,
                   uint8_t initialValue = default_char) {
    cli();
    
    SET_BIT(PIN_12, DDR, 1); // MISO
    
    _spi::_init<_spi::Mode::Slave, DataOrder, DataMode, 1>();
    _setDD(initialDd);
    SPDR = initialValue;
    sei();
  }
  
  static void reset(_spi::DataDirection newDd = _spi::DataDirection::ReadWrite,
                    uint8_t initialValue = default_char) {
    // clear all buffers and set data direction
    tx_buffer::clear();
    _getStatus() = 0;
    _setDD(newDd);
    SPDR = initialValue;
  }
  
  static void flush() {
    while (!tx_buffer::is_empty());
  }
  
  static uint8_t has_error(_spi::ErrorCheck errorCheck = _transmission::ErrorCheck::Now) {
    if (errorCheck == _transmission::ErrorCheck::FlushFirst) flush();
    return 0;
    //return _getStatus() & 0b10000000;
  }
  
  // implement IRQ-handler interface
  // this handle function handles 1 irq SPI_STC
  template <class I>
  static inline void handle(I) {
    const auto _dd = _getDD();
    
    // real dd might change in callbacks → use cached value (_dd)
    if (_dd == _spi::DataDirection::Read ||
        _dd == _spi::DataDirection::ReadOnly ||
        _dd == _spi::DataDirection::ReadWrite) {
      uint8_t received = SPDR;
      const auto toBuffer = IrqTask::template rx_complete<_SpiSlave>(received);
      if (toBuffer == _spi::ToBuffer::Yes && !rx_buffer::is_full()) rx_buffer::push(received);
    } 
    if (_dd == _spi::DataDirection::Write ||
        _dd == _spi::DataDirection::WriteOnly ||
        _dd == _spi::DataDirection::ReadWrite) {
      IrqTask::template tx_complete<_SpiSlave>();
    } 
    
    // __tx() will reset _transmitting to 1 if we send something.
    // loop for marked bytes  (note the break at the end → only possibility to stay in loop is continue)
    for (;;) {
      // dd might now be different to _dd (because of IrqTask or marked byte in loop)
      const auto dd = _getDD();
      if (dd == _spi::DataDirection::Write ||
          dd == _spi::DataDirection::WriteOnly ||
          dd == _spi::DataDirection::ReadWrite) {
        if (tx_buffer::is_empty()) IrqTask::template tx_empty<_SpiSlave>();
        
        // buffer might be filled by IrqTask → retry
        if (!tx_buffer::is_empty()) {
          uint8_t marked;
          auto c = tx_buffer::pop(marked);
          __tx(c, marked);
          if (marked) continue;
        }
      }
      break;
    }
  }
  
  static void sleep_until_data_available() {
    _transmission::sleep_until_data_available<rx_buffer>();
  }
  
  static void rx(uint8_t& c) {
    if (!can_rx()) sleep_until_data_available();
    c = _rx();
  }
  
  template <typename T>
  static void tx(const T& t) {
    Adapter::apply<_SpiSlave::_tx>(t);
  }
  
  static void tx(_spi::DataDirection dd) {
    _tx((uint8_t) dd, 1);
  }
  
  static void tx(_spi::ClearErrors e) {
    // don't do anything.
  }
  
  inline _SpiSlave& operator>>(uint8_t& c) {
    rx(c);
    return *this;
  }

  template <typename T>
  inline _SpiSlave& operator<<(const T& t) {
    tx(t);
    return *this;
  }
};

namespace _twi {
  void usageExample1() {
    typedef TwiMaster<16, 3> Master;
    
    // initialize the Twi system
    Master::init();
    
    // intantiate the SpiMaster class.  Note that the compiler will optimize this
    // so that no object is created at all.
    auto twiMaster = Master();
    
    const uint8_t lm75b_address = 0b1001101;
    // _twi::W will also switch DataDirection to Write
    twiMaster << _twi::W(lm75b_address);
    twiMaster << 0x00; // set reading register to 0 (temperatur for LM75B)
    // because we haven't closed the communication (EndFrame)
    twiMaster << _twi::R(lm75b_address);
    int16_t temp;
    twiMaster >> temp;
    if (twiMaster.has_error(_twi::ErrorCheck::FlushFirst)) {
      // something went wrong
    } else {
      twiMaster << _twi::DataDirection::EndFrame;
    }
    twiMaster.reset(); // clear all buffers, states...
    
    // int16_t >> 8 contains temperature in C
    // bit 7::5 of LSByte are 0.125°C resolution
  }
  
  class DataProviderTask {
  private:
    static uint8_t& n() {
      static uint8_t n = 0;
      return n;
    }

  public:
    template <typename Spi>
    static _spi::ToBuffer rx_complete(uint8_t c) {
      n() = c;
      Spi() << _spi::DataDirection::Write;
      return _spi::ToBuffer::No;
    }
    
    template <typename Spi>
    static void tx_complete() {
      Spi() << _spi::DataDirection::EndFrame;
      Spi() << _spi::DataDirection::Read;
    }
    
    template <typename Spi>
    static void tx_empty() {
      // called right after rx_complete switched to write mode
      // send n once then stop frame
      Spi() << (n() + 1);
    }
  };
  
  void usageExample2() {
    typedef SpiSlave<0, 0, DataProviderTask> SSlave;
    
    SSlave::init(_spi::DataDirection::Read, 255); // set initial transmission char
  }
}

#define REGISTER_SPI "internal/register_spi.h"
