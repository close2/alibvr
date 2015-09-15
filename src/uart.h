#pragma once

#ifndef F_CPU
#  error F_CPU not defined
#endif

#include "buffer.h"
#include "irqs.h"
#include "tasks.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "pgmspace.h"
#include "internal/adapter.h"
#include "transmission.h"

#pragma push_macro("BAUD_TOL")
#ifndef BAUD_TOL
#  define BAUD_TOL 2
#endif

namespace _uart {
  const uint32_t Cpu = F_CPU;
  
  enum Parity {
    No = 0,
    Odd = (2 << UPM00),
    Even = (3 << UPM00)
  };
  
  enum CharacterSize {
    // DataBits9, // not yet supported  (mostly because reading is not possible)
    DataBits5 = 0,
    DataBits6 = (1 << UCSZ00),
    DataBits7 = (2 << UCSZ00),
    DataBits8 = (3 << UCSZ00)
  };
  
  enum StopBits {
    StopBits1 = 0,
    StopBits2 = (1 << USBS0)
  };

  class DoNothing {
  public:
    static void rx(const uint8_t c) {
    }
    static uint8_t tx_pop() {
      return 0;
    }
    
    static uint8_t tx_is_empty() {
      return 1;
    }
    
    // allow a task to continue sending without having a typedef of Uart
    static void tx_starter(void (*f)()) {
    }
    
    static void tx_done() {
    }
  };
}

template <uint16_t baud,
          uint8_t _tx_buffer_size,
          uint8_t _rx_buffer_size,
          uint8_t use_irqs,
          class IrqTask = _uart::DoNothing, // Task to run in irq after having received a character
          _uart::StopBits stop_bits = _uart::StopBits2, // stop_bits are ignored by the receiver, you should probably always leave them at 2
          _uart::Parity parity_bit = _uart::No,
          _uart::CharacterSize data_bits = _uart::DataBits8,
          uint8_t baud_tol = BAUD_TOL>
class Uart {
private:
  // don't use a buffer if we don't have irqs enabled
  static const uint8_t tx_buffer_size = use_irqs ? _tx_buffer_size : 0;
  // force rx_buffer if we use irqs
  static const uint8_t rx_buffer_size = !use_irqs ? _rx_buffer_size : (_rx_buffer_size > 0 ? _rx_buffer_size : 1);
  
  class Uart_Tx_Buffer_Id {};
  typedef Static_Buffer<tx_buffer_size, Uart_Tx_Buffer_Id> tx_buffer;
  class Uart_Rx_Buffer_Id {};
  typedef Static_Buffer<rx_buffer_size, Uart_Rx_Buffer_Id> rx_buffer;
  
  inline static uint8_t can_tx_now() {
    return UCSR0A & _BV(UDRE0);
  }

  inline static void tx_now(const uint8_t c) {
    while (!can_tx_now()) {} // loop until bit is set
    UDR0 = c;
  }

  static void _tx(const uint8_t c) {
    if (tx_buffer_size == 0) {
      tx_now(c);
      return;
    }
    
    while(use_irqs && tx_buffer::is_full()) {
      // should already be enabled, but won't hurt
      tx_irq();
    }

    tx_buffer::push(c);
    if (use_irqs) {
      // turn on transmitting with irqs
      tx_irq();
    }
  }

  inline static uint8_t can_rx_now() {
    return UCSR0A & _BV(RXC0);
  }

  inline static uint8_t rx_now() {
    while (!can_rx_now()) {} // loop until bit is set
    return UDR0;
  }

  static uint8_t _rx() {
    if (use_irqs) {
      return rx_buffer::pop();
    }

    return rx_now();
  }

public:
  typedef Uart<baud, _tx_buffer_size, rx_buffer_size, use_irqs, IrqTask, stop_bits, parity_bit, data_bits, baud_tol> _Uart;

  static uint8_t can_rx() {
    if (use_irqs) {
      return !rx_buffer::is_empty();
    }
    return can_rx_now();
  }
  
  static uint8_t can_tx() {
    if (tx_buffer_size == 0) {
      return can_tx_now();
    }
    return !tx_buffer::is_full();
  }

  inline static void tx_irq() {
    UCSR0B |= _BV(UDRIE0);
  }
  
  static void init() {
    cli();

    uint16_t ubrr_value = (_uart::Cpu + 8UL * baud) / (16UL * baud) -1;
    const uint8_t use_2x = (100 * _uart::Cpu > (16 * (ubrr_value + 1)) * (100 * baud + baud * baud_tol)) ? 1 : 0;
    if (use_2x) {
      ubrr_value = ((_uart::Cpu + 4UL * baud) / (8UL * baud) -1UL);
    }
    const uint8_t ubrrl_value = ubrr_value & 0xff;
    const uint8_t ubrrh_value = ubrr_value >> 8;
    
    UBRR0H = ubrrh_value;
    UBRR0L = ubrrl_value;
    
    if (use_2x) {
      UCSR0A |= _BV(U2X0);
    } else {
      UCSR0A &= ~_BV(U2X0);
    }
    
    if (100 * _uart::Cpu > (8 * (ubrr_value + 1)) * (100 * baud + baud * baud_tol)) {
      // baud rate achieved is higher
    } else if (100 * (_uart::Cpu) < (8 * (ubrr_value + 1)) * (100 * baud - baud * baud_tol)) {
      // baud rate achieved is lower
    }
    
    // Frame format configuration:
    uint8_t conf = 0; // asynchronous USART is 00
    //  - Stop bits:
    conf |= stop_bits;
    //  - Data bits:
    conf |= data_bits;
    //  - Parity bit:
    conf |= parity_bit;
    UCSR0C = conf;
    // Frame format configuration end

    // enable both, receiver and transmitter
    UCSR0B |= _BV(RXEN0) | _BV(TXEN0);
    
    if (use_irqs) {
      // enable RX complete interrupt
      UCSR0B |= _BV(RXCIE0);
      IrqTask::tx_starter(_Uart::tx_irq);
    }
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

#define REGISTER_UART "internal/register_uart.h"
#pragma pop_macro("BAUD_TOL")
