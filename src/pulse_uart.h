#pragma once

#ifndef F_CPU
#  error F_CPU not defined
#endif

#include "macros.h"
#include "buffer.h"
#include "irqs.h"
#include "tasks.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "pgmspace.h"
#include "internal/adapter.h"
#include "transmission.h"
#include "type_traits.h"


/**
 * Transmission will usually be synchronous.  Pulse Uart has been implemented
 * to achieve higher speeds.  Transmission should therefore not take a lot of
 * time and IRQs would make an implementation probably much slower.
 * 
 * However the core transmission code can be used to transmit using IRQs.
 * (mini-tasks).
 * 
 * We have a function which returns in how many units it wants to be called
 * again.
 * 
 * The synchronous version will busy wait.  The IRQ version could use mini
 * tasks.
 * 
 * Reception OTOH is probably always IRQ based.
 **/


namespace _pulse_uart {
  const uint32_t Cpu = F_CPU;
  
  class DoNothing {
  public:
    static void rx(const uint8_t c) {
    }
  };
}


template <typename TxPin,
          uint16_t zeroBitDuration,
          uint8_t _tx_buffer_size,
          class Task = _pulse_uart::DoNothing, // Task to run after having sent a character,
          uint16_t oneBitDuration = (zeroBitDuration * 3) / 2,
          uint16_t syncBitDuration = zeroBitDuration * 2,
          uint8_t inverseOutput = 0>
          // uint8_t use_irqs = 0>  remove for now. maybe implement later
class PulseUartTx {
private:
  
  // should really find a better way
  typedef PulseUartTx<TxPin, zeroBitDuration, _tx_buffer_size, Task, oneBitDuration, syncBitDuration> _PulseUartTx;
  
  // making the high duration half the shortest duration (zero_duration)
  // should hopefully work.
  // We might have to adjust this, if our RC-modules stretch high / low
  // and throughput would be higher with another value.
  static const uint16_t _high_duration = zeroBitDuration / 2;
  static const uint16_t _zero_low_duration = zeroBitDuration - _high_duration;
  static const uint16_t _one_low_duration = oneBitDuration - _high_duration;
  static const uint16_t _sync_low_duration = syncBitDuration - _high_duration;
  
  static const uint8_t _low_output = inverseOutput ? 1 : 0;
  static const uint8_t _high_output = inverseOutput ? 0 : 1;
  
  // prepare for IRQ implementation.  Currently tx_buffer_size is always _tx_buffer_size
  static const uint8_t tx_buffer_size = _tx_buffer_size;
  
  class Pulse_Uart_Tx_Buffer_Id {};
  typedef Static_Buffer<tx_buffer_size, Pulse_Uart_Tx_Buffer_Id> tx_buffer;
  
  // this is the byte we are currently transmitting
  // note that we return a reference not a copy!
  // this byte will be shifted during transmission
  static uint8_t& get_tx_byte() {
    static uint8_t tx_byte = 0;
    return tx_byte;
  }
  
  // for now use 1 byte for storing the current bit location
  // we will probably merge this with a status.
  // current bit is shifted one to left
  // least significant bit is used to know if we need to send low or high value
  static uint8_t& get_tx_bit_counter() {
    static uint8_t tx_bit_counter = 0;
    return tx_bit_counter;
  }
  
  /*
   * every byte expects the line to have been high for at least the common
   * high duration
   *
   *  ↓ bit counter when entering _tx_bit_now 
   * 00000 startBit  high → low
   * 00001 startBit  low  → high
   * 00010 bit 1     high → low
   * 00011 bit 1     low  → high
   * 0010x bit 2     high → low → high
   * 0011x bit 3     high → low → high
   * 0100x bit 4     high → low → high
   * 0101x bit 5     high → low → high
   * 0110x bit 6     high → low → high
   * 0111x bit 7     high → low → high
   * 1000x bit 8     high → low → high
   * 10010 ensure high duration
   * 
   */
  
  // ⇒ uart is usually high in idle mode.
  static uint16_t _tx_bit_now() {
    // we will send 3 different values: 0, 1 and sync bit.
    // let's make the high always the same duration and change only the low
    // duration.
    // ----___-_-__-___---  would be sync bit, 0, 1 and sync bit
    // ___- ... sync bit
    // _-   ... 0
    // __-  ... 1
    uint8_t& tx_bit_counter = get_tx_bit_counter();
    
    // prepare for combined bit_counter with status bits
    const uint8_t bit_counter_mask = 0b11111;
    const uint8_t current_bit_counter = tx_bit_counter & bit_counter_mask;
    const uint8_t end_counter = 0b10010; // see comment above
    
    uint8_t& tx_byte = get_tx_byte();
    
    // we will send the sync byte before every byte transmitted
    // this means, that we have to count to 9 (see comment above)
    uint16_t ret;
    if (current_bit_counter == end_counter) {
      ret = 0; // we don't go low after last bit
    } else if ((current_bit_counter & 1) == 0) {
      // send low (different durations depending on value)
      SET_BIT(TxPin, PORT, _low_output);
      
      if (current_bit_counter == 0) ret = _sync_low_duration;
      else {
        if (tx_byte & 1) ret = _one_low_duration;
        else ret = _zero_low_duration;
        tx_byte >>= 1;
      }
    } else {
      // send high (always the same duration)
      SET_BIT(TxPin, PORT, _high_output);
      ret = _high_duration;
    }
    
    tx_bit_counter = tx_bit_counter & ~bit_counter_mask;
    if (current_bit_counter != end_counter) {
      tx_bit_counter += current_bit_counter + 1;
    }
    
    return ret;
  }
  
  static void _tx_now() {
    uint16_t callIn;
    uint16_t previousClock = get_clock<uint16_t>();
    
    while ((callIn = _tx_bit_now()) != 0) {
      const uint16_t nextClock = previousClock + callIn;

      while (!clock_reached<uint16_t>(previousClock, nextClock)); // busy wait

      // the duration of one bit is the low duration together with
      // the high duration.
      // So we _should_ set the previousClock to the nextClock when
      // being in the middle of transfering one bit.
      // Between different bits we _should_ call get_clock, but
      // in this function we don't really know if we are in the middle
      // of transmitting a bit, or in between 2 different bits, so we
      // will use the cheaper version: nextClock
      previousClock = nextClock;

    }
    // byte has been transmitted
  }
  
  static void _tx(uint8_t c) {
    get_tx_byte() = c;
    _tx_now();
  }
  
public:
  static void init() {
    SET_BIT(TxPin, DDR, 1);
  }

  template <typename T>
  static void tx(const T& t) {
    Adapter::apply<_PulseUartTx::_tx>(t);
  }

  template <typename T>
  inline _PulseUartTx& operator<<(const T& t) {
    tx(t);
    return *this;
  }
};


/*
template <uint16_t zeroBitDuration,
          uint8_t _rx_buffer_size,
          class IrqTask = _pulse_uart::DoNothing, // Task to run in irq after having received a character
          uint16_t oneBitDuration = zeroBitDuration * 1.5,
          uint16_t syncBitDuration = zeroBitDuration * 2,
          uint8_t use_irqs = (_rx_buffer_size > 0 ||
                              !std::is_same< IrqTask, _pulse_uart::DoNothing>()) >
class PulseUartRx {
private:
  
  // force rx_buffer if we use irqs
  static const uint8_t rx_buffer_size = !use_irqs ? _rx_buffer_size : (_rx_buffer_size > 0 ? _rx_buffer_size : 1);
  
  class Pulse_Uart_Rx_Buffer_Id {};
  typedef Static_Buffer<rx_buffer_size, Pulse_Uart_Rx_Buffer_Id> rx_buffer;
  
  
  // rx_bytes[0] holds (incomplete) byte we are currently receiving
  // which will be copied to rx_bytes[1] (if status == DataRead or Idle)
  // rx_bytes[1] contains completely received byte.
  static uint8_t& rx_bytes() {
    static uint8_t rx_bytes[2];
    return rx_bytes;
  }
  
  enum Status {
    DataReady,
    DataRead,
    Idle
  };
  
  static Status& status() {
    static Status status = Idle;
    return status;
  }
  
  
  inline static uint8_t can_rx_now() {
    return status() == Status::DataReady;
  }

  inline static uint8_t rx_now() {
    while (status() != Status::DataReady) {} // loop until data is available
    const uint8_t data = rx_bytes()[1];
    status() = DataRead;
    return data;
  }

  static uint8_t _rx() {
    if (use_irqs) {
      return rx_buffer::pop();
    }

    return rx_now();
  }

public:
  static uint8_t can_rx() {
    if (use_irqs) {
      return !rx_buffer::is_empty();
    }
    return can_rx_now();
  }
  
  static void init() {
    cli();
    sei();
  }
  
  // implement IRQ-handler interface
  // this handle function handles 2 irqs USART_RX and USART_UDRE!
  template <class I>
  static inline void handle(I) {
    if (!use_irqs) return;

    if (I::irq == _irqs::IRQ_USART_RX) {
      // we need to first store the UCSR0A before reading from UDR0 because
      // the receiver could possibly overwrite UCSR0A with the status for
      // the next byte (which will be received as soon as we read from UDR0)
      const uint8_t status = UCSR0A & (_BV(FE0) | _BV(DOR0) | _BV(UPE0));
      const uint8_t c = UDR0;
      if (status == 0) {
        if (rx_buffer_size > 0) {
          rx_buffer::push(c);
        }
        IrqTask::rx(c);
      }
    } else {
      // _irqs::IRQ_USART_UDRE
      uint8_t c;
      if (!IrqTask::tx_is_empty()) {
        c = IrqTask::tx_pop();
      } else {
        if (tx_buffer_size == 0 || tx_buffer::is_empty()) {
          UCSR0B &= ~_BV(UDRIE0);
          IrqTask::tx_done();
          return;
        }
        c = tx_buffer::pop();
      }
      
      UDR0 = c;
    }
  }
  
  static void sleep_until_data_available() {
    if (!use_irqs) {
      while (rx_buffer::is_empty()) {};
      return;
    }

    _transmission::sleep_until_data_available<rx_buffer>();
  }
  
  static void rx(uint8_t& c) {
    sleep_until_data_available();
    c = _rx();
  }
  
  inline _Uart& operator>>(uint8_t& c) {
    rx(c);
    return *this;
  }

  template <typename T>
  static void tx(const T& t) {
    Adapter::apply<_Uart::_tx>(t);
  }

  template <typename T>
  inline _Uart& operator<<(const T& t) {
    tx(t);
    return *this;
  }
};


namespace _pulse_uart {
  
  void usageExample1() {
    const auto zero_duration = ms_to_units<>(100);
    typedef PulseUartTx<zero_duration> Tx;
    
    Tx::init();
    
    auto tx = Tx();
    
    tx << "hello";
  }
  
  void usageExample2() {
    const auto zero_duration = ms_to_units<>(100);
    const auto buffer_size = 10;
    
    typedef PulseUartRx<zero_duration, buffer_size> Rx;
    
    Rx::init();
    
    auto rx = Rx();
    
    char hello[8];
    rx >> hello;
    
    
    uint8_t success = (hello[0] == 'h' &&
                       hello[1] == 'e' &&
                       hello[2] == 'l' &&
                       hello[3] == 'l' &&
                       hello[4] == 'o' &&
                       hello[5] == '\0');
    
  }
  
  class DataReceiverTask {
  private:
    static uint16_t& sum() {
      static uint16_t sum = 0;
      return sum;
    }

  public:
    static __pulse_uart::ToBuffer rx_complete(uint8_t c) {
      sum() += c;
      return _pulse_uart::ToBuffer::No;
    }
  };
  
  void usageExample3() {
    const auto zero_duration = ms_to_units<>(100);
    const auto buffer_size = 0;
    typedef PulseUartRx<zero_duration, buffer_size, 1, DataReceiverTask> Rx;
    
    Rx::init();
  }
}
*/

// FIXME (add register_pulse_uart.h)
//#define REGISTER_UART "internal/register_uart.h"
