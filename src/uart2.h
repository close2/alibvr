// define UART_USE_INTERRUPTS if you want to use interrupts

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdio.h>

#include "macros.h"

#ifdef _DEVICE_NB
#  undef _DEVICE_NB
#endif

#ifndef UART_NB
#  define UART_NB 0
#endif

#define _DEVICE_NB UART_NB

#define _INNER_BUILD_NAME2(pre, n, ...) pre##n##__VA_ARGS__
#define _INNER_BUILD_NAME(pre, n, post, ...) _INNER_BUILD_NAME2(pre##__VA_ARGS__, n, post##__VA_ARGS__)
#define _BUILD_NAME(pre, post, ...) _INNER_BUILD_NAME(pre##__VA_ARGS__, _DEVICE_NB, post##__VA_ARGS__)

#define _INNER_BUILD_NAME_S(name, n, ...) _INNER_BUILD_NAME2(name##__VA_ARGS__, n)
#define _BUILD_NAME_S(name, ...) _INNER_BUILD_NAME_S(name##__VA_ARGS__, _DEVICE_NB)

// -- registers --
// uart baud rate register (high and low)
#define _UBRRH _BUILD_NAME(UBRR, H)
#define _UBRRL _BUILD_NAME(UBRR, L)
// uart control and status register A B and C
#define _UCSRA _BUILD_NAME(UCSR, A)
#define _UCSRB _BUILD_NAME(UCSR, B)
#define _UCSRC _BUILD_NAME(UCSR, C)
// uart data register
#define _UDR _BUILD_NAME_S(UDR)
// ----

// -- bit positions --
// uart double speed
#define _U2X _BUILD_NAME_S(U2X)
// receiver enable
#define _RXEN _BUILD_NAME_S(RXEN)
// transmitter enable
#define _TXEN _BUILD_NAME_S(TXEN)
// frame error
#define _FE _BUILD_NAME_S(FE)
// data over run
#define _DOR _BUILD_NAME_S(DOR)
// parity error
#define _UPE _BUILD_NAME_S(UPE)
// receive complete
#define _RXC _BUILD_NAME_S(RXC)
// uart data register empty
#define _UDRE _BUILD_NAME_S(UDRE)
// RX complete interrupt
#define _RXCIE _BUILD_NAME_S(RXCIE)
// data register empty interrupt
#define _UDRIE _BUILD_NAME_S(UDRIE)
// uart stop bit select
#define _USBS _BUILD_NAME_S(USBS)
// ----

#define _INIT_UART _BUILD_NAME_S(uart_init)
#define _GET_CHAR _BUILD_NAME_S(uart_get_char)
#define _PUT_CHAR _BUILD_NAME_S(uart_put_char)

#ifdef UART_USE_INTERRUPTS

#  if defined USART_RX_vect && UART_NB == 0
#    define _USART_RX_vect USART_RX_vect
#    define _USART_UDRE_vect USART_UDRE_vect
#  elif defined USART0_RX_vect
#    define _USART_RX_vect _BUILD_NAME(USART, _RX_vect)
#    define _USART_UDRE_vect _BUILD_NAME(USART, _UDRE_vect)
#  elif defined USART0_RXC_vect
#    define _USART_RX_vect _BUILD_NAME(USART, _RXC_vect)
#    define _USART_UDRE_vect _BUILD_NAME(USART, _UDRE_vect)
#  elif defined UART_RX_vect && UART_NB == 0
#    define _USART_RX_vect UART_RX_vect
#    define _USART_UDRE_vect UART_UDRE_vect
#  elif defined UART0_RX_vect
#    define _USART_RX_vect _BUILD_NAME(UART, _RX_vect)
#    define _USART_UDRE_vect _BUILD_NAME(UART, _UDRE_vect)
#  else
#    error "Could not construct uart interrupt names"
#  endif

#  define _UART_RX_BUFFER _BUILD_NAME_S(uart_rx_buffer)
#  define _UART_TX_BUFFER _BUILD_NAME_S(uart_tx_buffer)

#  define _CHARACTERS_AVAILABLE _BUILD_NAME_S(uart_data_available)
#  define _SLEEP_UNTIL_DATA_AVAILABLE _BUILD_NAME_S(uart_sleep_until_data_available)

#  if ! defined UART_RX_BUFFER_SIZE && ! defined UART_BUFFER_SIZE
#    error "Either UART_RX_BUFFER_SIZE or UART_BUFFER_SIZE must be defined in interrupt mode"
#  endif
#  ifndef UART_RX_BUFFER_SIZE
#    define UART_RX_BUFFER_SIZE UART_BUFFER_SIZE
#  endif

#  if ! defined UART_TX_BUFFER_SIZE && ! defined UART_BUFFER_SIZE
#    error "Either UART_TX_BUFFER_SIZE or UART_BUFFER_SIZE must be defined in interrupt mode"
#  endif
#  ifndef UART_TX_BUFFER_SIZE
#    define UART_TX_BUFFER_SIZE UART_BUFFER_SIZE
#  endif

#  include <avr/sleep.h>
#  include <avr/interrupt.h>

volatile static struct {
  uint8_t head;
  uint8_t tail;
  uint8_t buffer[UART_RX_BUFFER_SIZE];
} _UART_RX_BUFFER;

ISR(_USART_RX_vect) {
  // we need to first store the _UCSRA before reading from _UDR because
  // the receiver could possibly overwrite _UCSRA with the status for
  // the next byte (which will be received as soon as we read from _UDR)
  uint8_t status = _UCSRA & (_BV(_FE) | _BV(_DOR) | _BV(_UPE));
  uint8_t c = _UDR;
  if (status == 0) {
    uint8_t i = (_UART_RX_BUFFER.head + 1) % UART_RX_BUFFER_SIZE;
    // only insert new character into buffer if we wouldn't
    // overwrite already existing characters in our buffer
    // this could of course mean, that we possibly lose data
    if (i != _UART_RX_BUFFER.tail) {
      _UART_RX_BUFFER.buffer[_UART_RX_BUFFER.head] = c;
      _UART_RX_BUFFER.head = i;
    }
  }
}

static uint8_t _CHARACTERS_AVAILABLE() {
  return (UART_RX_BUFFER_SIZE + _UART_RX_BUFFER.tail - _UART_RX_BUFFER.head) % UART_RX_BUFFER_SIZE;
}

static void _SLEEP_UNTIL_DATA_AVAILABLE() {
  uint8_t sreg;
  sreg = SREG;
  
  cli();
  set_sleep_mode(SLEEP_MODE_IDLE);
  while (_UART_RX_BUFFER.head == _UART_RX_BUFFER.tail) {
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
    cli();
  }
  
  SREG = sreg;
}

static int _GET_CHAR(FILE* unused) {
  
  while (_UART_RX_BUFFER.head == _UART_RX_BUFFER.tail) {
    // don't do anything, wait for interrupt to fill the buffer again
  }
  
  uint8_t c = _UART_RX_BUFFER.buffer[_UART_RX_BUFFER.tail];
  _UART_RX_BUFFER.tail = (_UART_RX_BUFFER.tail + 1) % UART_RX_BUFFER_SIZE;
  return c;
}

volatile static struct {
  uint8_t head;
  uint8_t tail;
  uint8_t buffer[UART_TX_BUFFER_SIZE];
} _UART_TX_BUFFER;

ISR(_USART_UDRE_vect) {
  if (_UART_TX_BUFFER.head == _UART_TX_BUFFER.tail) {
    _UCSRB &= ~_BV(_UDRIE);
    return;
  }
  
  uint8_t c = _UART_TX_BUFFER.buffer[_UART_TX_BUFFER.tail];
  _UART_TX_BUFFER.tail = (_UART_TX_BUFFER.tail + 1) % UART_TX_BUFFER_SIZE;
  _UDR = c;
}

static int _PUT_CHAR(char c, FILE* unused) {
  if (c == '\n') {
    if (_PUT_CHAR('\r', unused) != 0) {
      return 1;
    }
  }
  
  uint8_t i;
  i = (_UART_TX_BUFFER.head + 1) % UART_TX_BUFFER_SIZE;
  
  // if buffer is full, wait till there is room:
  while (i == _UART_TX_BUFFER.tail) {
    // just to be sure, turn on interrupt (should not be necessary)
    _UCSRB |= _BV(_UDRIE);
  }
  // first write the new value
  _UART_TX_BUFFER.buffer[_UART_TX_BUFFER.head] = c;
  // then update head.
  // (Otherwise data interrupt could occur and write wrong value)
  _UART_TX_BUFFER.head = i;
  // turn on usart data interrupt.
  _UCSRB |= _BV(_UDRIE);
  return 0;
}

#else
static int _GET_CHAR(FILE* unused) {
  loop_until_bit_is_set(_UCSRA, _RXC);
  return _UDR;
}

static int _PUT_CHAR(char c, FILE* unused) {
  if (c == '\n') {
    _PUT_CHAR('\r', unused);
  }
  loop_until_bit_is_set(_UCSRA, _UDRE);
  _UDR = c;
  return 0;
}
#endif


#pragma push_macro("BAUD_TOL")
#ifndef BAUD_TOL
#  define BAUD_TOL 2
#endif
template <uint16_t TBaud, uint32_t TCpu, uint8_t TTol>
static void _INIT_UART() {
#ifdef F_CPU
  const uint32_t Cpu = TCpu == 0 ? F_CPU : TCpu;
#else
  const uint32_t Cpu = TCpu;
#endif
#ifdef BAUD
  const uint16_t Baud = TBaud == 0 ? BAUD : TBaud;
#else
  const uint16_t Baud = TBaud;
#endif
  const uint16_t Tol = TTol == 0 ? BAUD_TOL : TTol;

  cli();
  
  uint16_t ubrr_value = (Cpu + 8UL * Baud) / (16UL * Baud) -1;
  const uint8_t use_2x = (100 * Cpu > (16 * (ubrr_value + 1)) * (100 * Baud + Baud * Tol)) ? 1 : 0;
  if (use_2x) {
    ubrr_value = ((Cpu + 4UL * Baud) / (8UL * Baud) -1UL);
  }
  const uint8_t ubrrl_value = ubrr_value & 0xff;
  const uint8_t ubrrh_value = ubrr_value >> 8;
  
  _UBRRH = ubrrh_value;
  _UBRRL = ubrrl_value;
  
  if (use_2x) {
    _UCSRA |= _BV(_U2X);
  } else {
    _UCSRA &= ~_BV(_U2X);
  }

  if (100 * Cpu > (8 * (ubrr_value + 1)) * (100 * Baud + Baud * Tol)) {
    // baud rate achieved is higher
  } else if (100 * (Cpu) < (8 * (ubrr_value + 1)) * (100 * Baud - Baud * Tol)) {
    // baud rate achieved is lower
  }

  // enable both, receiver and transmitter
  _UCSRB |= _BV(_RXEN) | _BV(_TXEN);
  // enable 2 stop bits (ignored by receiver and hopefully makes transmitting more stable)
  _UCSRC |= _BV(_USBS);
#ifdef UART_USE_INTERRUPTS
  // enable RX complete interrupt
  _UCSRB |= _BV(_RXCIE);
  sei();
#endif
}

#pragma pop_macro("BAUD_TOL")