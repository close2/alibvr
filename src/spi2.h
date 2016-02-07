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
namespace _spi {
  
  enum class Mode {
    Master,
    Slave
  };
  
  enum class DataOrder {
    LSB,
    MSB
  };
  
  enum class DataMode {
    Mode0, // Leading Edge: Sample (Rising);  Trailing Edge: Setup  (Falling)
    Mode1, // Leading Edge: Setup  (Rising);  Trailing Edge: Sample (Falling)
    Mode2, // Leading Edge: Sample (Falling); Trailing Edge: Setup  (Rising)
    Mode3, // Leading Edge: Setup  (Falling); Trailing Edge: Sample (Rising)
  };
  
  typedef PIN_10 PI_SS;
  
  class NoSS {
  };
  
  typedef _clock_select::ClockSelect ClockSelect;
  
  typedef _transmission::DataDirection DataDirection;
  
  typedef _transmission::ErrorCheck ErrorCheck;
  
  typedef _transmission::ToBuffer ToBuffer;
  
  typedef _transmission::ClearErrors ClearErrors;
  

  // TODO combine uart and spi DoNothing and move outside
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
  
  
  template <ClockSelect CS,
            Mode M,
            DataOrder DO,
            DataMode DM,
            int8_t useIrqs = 0>
  static void _init() {
    uint8_t spcr = 0;
    uint8_t spsr = 0;
    
    spcr |= _BV(SPE);
    if (useIrqs) spcr |= _BV(SPIE);
    
    if (M == Mode::Master) {
      spcr |= _BV(MSTR);
    }
    
    if (DO == DataOrder::LSB) {
      spcr |= _BV(DORD);
    }
    
    if (DM == DataMode::Mode2 ||
        DM == DataMode::Mode3) {
      spcr |= _BV(CPOL);
    }
    if (DM == DataMode::Mode1 ||
        DM == DataMode::Mode3) {
      spcr |= _BV(CPHA);
    }
        
    static_assert(CS == ClockSelect::Clock2 ||
                  CS == ClockSelect::Clock4 ||
                  CS == ClockSelect::Clock8 ||
                  CS == ClockSelect::Clock16 ||
                  CS == ClockSelect::Clock32 ||
                  CS == ClockSelect::Clock64 ||
                  CS == ClockSelect::Clock128,
                  "ClockSelect for SPI must be either 2, 4, 8, 16, 32, 64 or 128");
    
    switch (CS) {
      case ClockSelect::Clock2:
        spsr |= _BV(SPI2X);
        break;
      case ClockSelect::Clock4:
        // nothing to do
        break;
      case ClockSelect::Clock8:
        spsr |= _BV(SPI2X);
        spcr |= _BV(SPR0);
        break;
      case ClockSelect::Clock16:
        spcr |= _BV(SPR0);
        break;
      case ClockSelect::Clock32:
        spsr |= _BV(SPI2X);
        spcr |= _BV(SPR1);
        break;
      case ClockSelect::Clock64:
        spsr |= _BV(SPI2X);
        spcr |= _BV(SPR1) | _BV(SPR0);
        break;
      case ClockSelect::Clock128:
        spcr |= _BV(SPR1) | _BV(SPR0);
        break;
      default:
        break; // will never happen because of static_assert
    }
    
    SPCR = spcr;
    SPSR = spsr;
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
    _spi::_init<ClockSelect, Mode, DataOrder, DataMode>();
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


template <_spi::ClockSelect ClockSelect,
          _spi::Mode Mode,
          uint8_t tx_buffer_size,
          uint8_t rx_buffer_size,
          class IrqTask = _spi::DoNothing, // Task to run in irq after having received a character
          uint8_t default_char = 0xFF, // Send this in READ mode
          typename _SS = PIN_10, // in slave mode ignored
          _spi::DataOrder DataOrder = _spi::DataOrder::MSB,
          _spi::DataMode DataMode = _spi::DataMode::Mode0>
class SpiAsync {
private:
  
  typedef SpiAsync<ClockSelect, Mode, tx_buffer_size, rx_buffer_size, IrqTask, default_char, _SS, DataOrder, DataMode> _SpiAsync;
  
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

  // overwrite SS to SS-pin (PIN_10) if in slave mode
  typedef typename std::conditional<Mode == _spi::Mode::Slave, PIN_10, _SS>::type SS;


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
      const auto toBuffer = IrqTask::template rx_complete<_SpiAsync>(received);
      if (toBuffer == _spi::ToBuffer::Yes && !rx_buffer::is_full()) rx_buffer::push(received);
    } 
    if (_dd == (uint8_t) _spi::DataDirection::Write ||
        _dd == (uint8_t) _spi::DataDirection::WriteOnly ||
        _dd == (uint8_t) _spi::DataDirection::ReadWrite) {
      IrqTask::template tx_complete<_SpiAsync>();
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
        if (tx_buffer::is_empty()) IrqTask::template tx_empty<_SpiAsync>();
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
    Adapter::apply<_SpiAsync::_tx>(t);
  }
  
  static void tx(_spi::DataDirection dd) {
    _tx((uint8_t) dd, 1);
  }
  
  static void tx(_spi::ClearErrors e) {
    // don't do anything.
  }
  
  inline _SpiAsync& operator>>(uint8_t& c) {
    rx(c);
    return *this;
  }

  template <typename T>
  inline _SpiAsync& operator<<(const T& t) {
    tx(t);
    return *this;
  }
};


namespace _spi {
  void usageExample1() {
    typedef SpiAsync<_spi::ClockSelect::Clock8, Mode::Master, 16, 3> SpiMaster;
    
    // initialize the Spi system
    SpiMaster::init();
    
    // intantiate the SpiMaster class.  Note that the compiler will optimize this
    // so that no object is created at all.
    auto spiMaster = SpiMaster();
    
    spiMaster << _spi::DataDirection::Write;
    spiMaster << "Let us exchange 3 by";
    spiMaster.flush();
    spiMaster << "tes.";
    spiMaster << _spi::DataDirection::ReadWrite;
    spiMaster << "123";
    uint8_t bytes[3];
    spiMaster >> bytes[0];
    spiMaster >> bytes[1];
    spiMaster >> bytes[2];
    if (spiMaster.has_error(_spi::ErrorCheck::FlushFirst)) {
      // not good, maybe just try again.
    } else {
      spiMaster << _spi::DataDirection::EndFrame;   
    }
    spiMaster.reset();
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
    typedef SpiAsync<_spi::ClockSelect::Clock8, Mode::Slave, 0, 0, DataProviderTask> SpiSlave;
    
    SpiSlave::init(_spi::DataDirection::Read, 255); // set initial transmission char
  }
}

#define REGISTER_SPI "internal/register_spi.h"
