#pragma once

#include <avr/sfr_defs.h>
#include <avr/io.h>

#pragma push_macro("NS")
#ifndef ALIBVR_NAMESPACE_PORTS
#  ifdef ALIBVR_NAMESPACE_PREFIX
#    define NS ALIBVR_NAMESPACE_PREFIX ## ports
#  else
#    define NS ports
#  endif
#else
#  define NS ALIBVR_NAMESPACE_PORTS
#endif



/**
 * @brief Avr Atmegas access and control pins through registers.
 *        The registers are 8 bit wide and up to 8 pins are therefore
 *        controlled with every register.
 * 
 * * The DDR registers controls the data directions.  
 * * The PORT registers the output or the pull up resistors.  
 * * The PIN registers toggle the output or are used to read
 *   values from pins.
 * 
 * By default ports related classes, enums,... are defined
 * inside the `ports` namespace.  If this creates a name clash with your
 * code you may modify the namespace name by setting
 * ALIBVR_NAMESPACE_PORTS or ALIBVR_NAMESPACE_PREFIX.
 * 
 * The pin layout is compatible to arduino / arduino lite.
 * 
 * Prefix names in the following graph with `Pin_` to get
 * provided `typedef`s.  Example: `B5` becomes `Pin_B5`, `8` becomes `Pin_8`
 * 
 * «PORT_TYPEDEFS[^ \* ,]»
 * 
 * ```
 *                                  ┏━u━┓
 *                      C6    DIP_1 ┃   ┃ DIP_28   19   C5   ADC5   SCL
 * RXD              0   D0    DIP_2 ┃   ┃ DIP_27   18   C4   ADC4   SDA
 * TXD              1   D1    DIP_3 ┃   ┃ DIP_26   17   C3   ADC3
 *         INT0     2   D2    DIP_4 ┃   ┃ DIP_25   16   C2   ADC2
 * OC2B    INT1     3   D3    DIP_5 ┃   ┃ DIP_24   15   C1   ADC1
 * XCK     T0       4   D4    DIP_6 ┃   ┃ DIP_23   14   C0   ADC0
 *                              VCC ┃   ┃ GND
 *                              GND ┃   ┃ AREF
 * XTAL1   TOSC1   20   B6    DIP_9 ┃   ┃ AVCC
 * XTAL2   TOSC2   21   B7   DIP_10 ┃   ┃ DIP_19   13   B5   SCK
 * T1      OC0B     5   D5   DIP_11 ┃   ┃ DIP_18   12   B4   MISO
 * AIN0    OC0A     6   D6   DIP_12 ┃   ┃ DIP_17   11   B3   MOSI   OC2A
 * AIN1             7   D7   DIP_13 ┃   ┃ DIP_16   10   B2   SS     OC1B
 * ICP1    CLK0     8   B0   DIP_14 ┃   ┃ DIP_15    9   B1          OC1A
 *                                  ┗━━━┛
 * ```
 * ¤
 **/


namespace NS {
  
  /**
   * @brief Used in pin typedefs to specify the port of the pin.
   * 
   * Probably only useful internally.
   **/
  enum class _Port {
    B,
    C,
    D
  };
  
  /**
   * @brief Used as template argument to specify on which register some
   * function should be applied to.
   * 
   * Probably only useful internally.
   **/
  enum class _IOReg {
    DDRx,
    PORTx,
    PINx
  };
  
  /**
   * @brief Type safe enum for DataDirection.
   * 
   * When using this enum the compiler can verify that the correct
   * register is used (i.e. casts to and from this enum are only
   * implemented on the Pin::DDR variable.)
   * 
   * @see Pin::DDR
   * 
   * This enum is also used internally when one function implements
   * either a read or a write operation depending on this enum.
   **/
  enum class DataDirection {
    Read = 0,
    Write = 1,
    Input = Read,
    Output = Write,
    In = Read,
    Out = Write
  };
  
  /**
   * @brief Type safe enum for the internal Pull Up resistor.
   * 
   * @see Pin::PORT
   * 
   * When using this enum the compiler can verify that the correct
   * register is used (i.e. casts to and from this enum are only
   * implemented on the Pin::PORT variable.)
   **/
  enum class PullUp {
    Off = 0,
    On = 1,
    HighZ = Off
  };
  
  template <enum _Port p, uint8_t b, enum _IOReg io>
  struct _Io;
  
  /**
   * @brief Every pin has one ore multiple typedefs of this class.
   * 
   * The port name (enum _Port) and the bit in the registers is provided
   * as static variables.
   * 
   * In addition reading or setting the value of a pin is simplified
   * through #DDR #PORT and #PIN which have cast converters from uint8_t.
   * 
   * Using those static members (#DDR, #PORT or #PIN) liberates you from
   * writing bit operations.  If your pin is for instance B2, a typical
   * way to  set the DDR value of the pin to 0 would have been:
   * `PORTB &= ~(_BV(2));`.
   * 
   * The exact same thing may now be done by writing:
   * `PORT_B2::DDR = 0;`
   * 
   * When using the enums #DataDirection or #PullUp the compiler
   * verifies that the enum is used for the correct register.
   * 
   * The compiler does not verify that you are in the correct "mode".
   * Calling setPullUp() when the pin is in output mode will change the
   * output!  You would need to switch to input first
   * (`DDR = DataDirection::Input;`).
   **/
  template <enum _Port p, uint8_t b>
  struct Pin {
    
    /// @brief The port name (enum _Port) of this pin.
    static const enum _Port port = p;
    
    /// @brief The bit position (0-7) inside the DDRx, PORTx or PINx
    /// register.
    static const uint8_t   bit = b;
    
    
    typedef _Io<p, b, _IOReg::DDRx>  _DDR;
    
    /// @brief Simplified access to the DDR value of this pin.
    ///
    /// A cast operator of this variable allows you to read and write
    /// the DDR value for this bit as if every bit had its own `uint8_t`.
    ///
    /// Cast operators to and from enum #DataDirection are also implemented
    /// and prevent accidental use of an incorrect register.
    /// 
    /// Examples:
    ///
    /// * `PORT_C3::DDR = DataDirection::Input;`
    /// * `uint8_t currentDDR = PORT_C3::DDR;`
    /// * `enum DataDirection currentDDR2 = PORT_C3::DDR;`
    static _DDR DDR;
    
    
    typedef _Io<p, b, _IOReg::PORTx> _PORT;
    
    /// @brief Simplified access to the PORT value of this pin.
    ///
    /// A cast operator of this variable allows you to read and write
    /// the PORT value for this bit as if every bit had its own `uint8_t`.
    ///
    /// Cast operators to and from enum #PullUp are also implemented and
    /// prevent accidental use of an incorrect register.
    ///
    /// Examples:
    ///
    /// * `PORT_ADC4::PORT = 1;`
    /// * `uint8_t currentPort = PORT_ADC4::PORT;`
    /// * `enum PullUp currentPullUp = PORT_ADC4::PORT;`
    static _PORT PORT;
    
    
    typedef _Io<p, b, _IOReg::PINx>  _PIN;
    
    /// @brief Simplified access to the PIN value of this pin.
    ///
    /// A cast operator of this variable allows you to read and write
    /// the DDR value for this bit as if every bit had its own uint8_t.
    ///
    /// Examples:
    ///
    /// * `PORT_C3::DDR = DataDirection::Input;`
    /// * `uint8_t currentDDR = PORT_C3::DDR;`
    /// * `DataDirection currentDDR2 = PORT_C3::DDR;`
    static _PIN PIN;
    
    /// @brief Equivalent to `DDR = dd;`.
    static void setDD(const enum DataDirection dd) {
      DDR = (uint8_t) dd;
    }
    
    /// @brief returns either DataDirection::In or DataDirection::Output
    /// by reading from DDR and casting the bit value.
    static enum DataDirection getDD() {
      return DDR == 0 ? DataDirection::In : DataDirection::Out;
    }
    
    /// @brief Equivalent to `PORT = pullUp;`.
    static void setPullUp(const enum PullUp pullUp) {
      PORT = (uint8_t) pullUp;
    }
    
    /// @brief returns either PullUp::Off or PullUp::On
    /// by reading from PORT and casting the bit value.
    static enum PullUp getPullUp() {
      return PORT == 0 ? PullUp::Off : PullUp::On;
    }
    
    /// @brief Equivalent to first setting the data direction
    /// `DDR = DataDirection::Input;` followed by setting the pull up
    /// resistor: `PORT = pullup;`.
    static void setToInput(const enum PullUp pullUp) {
      setDD(DataDirection::Input);
      setPullUp(pullUp);
    }
    
    /// @brief Equivalent to `PIN = 1;` which toggles the output if
    /// pin is in output mode.
    static void toggle() { PIN = 1; }
  };
  
  // The compiler will optimize these instantiations away.
  // When using optimizations these definitions aren't even necessary.
  // In debug mode (i.e. with no optimizations) gcc will however complain
  // if we don't add them here.
  template <enum _Port p, uint8_t b> typename Pin<p, b>::_DDR Pin<p, b>::DDR;
  template <enum _Port p, uint8_t b> typename Pin<p, b>::_PORT Pin<p, b>::PORT;
  template <enum _Port p, uint8_t b> typename Pin<p, b>::_PIN Pin<p, b>::PIN;
}

typedef struct NS::Pin<NS::_Port::B, -1> PIN_UNUSED;

typedef struct NS::Pin<NS::_Port::D, 0 > PIN_0 ;
typedef struct NS::Pin<NS::_Port::D, 1 > PIN_1 ;
typedef struct NS::Pin<NS::_Port::D, 2 > PIN_2 ;
typedef struct NS::Pin<NS::_Port::D, 3 > PIN_3 ;
typedef struct NS::Pin<NS::_Port::D, 4 > PIN_4 ;
typedef struct NS::Pin<NS::_Port::D, 5 > PIN_5 ;
typedef struct NS::Pin<NS::_Port::D, 6 > PIN_6 ;
typedef struct NS::Pin<NS::_Port::D, 7 > PIN_7 ;
typedef struct NS::Pin<NS::_Port::B, 0 > PIN_8 ;
typedef struct NS::Pin<NS::_Port::B, 1 > PIN_9 ;
typedef struct NS::Pin<NS::_Port::B, 2 > PIN_10;
typedef struct NS::Pin<NS::_Port::B, 3 > PIN_11;
typedef struct NS::Pin<NS::_Port::B, 4 > PIN_12;
typedef struct NS::Pin<NS::_Port::B, 5 > PIN_13;
typedef struct NS::Pin<NS::_Port::C, 0 > PIN_14;
typedef struct NS::Pin<NS::_Port::C, 1 > PIN_15;
typedef struct NS::Pin<NS::_Port::C, 2 > PIN_16;
typedef struct NS::Pin<NS::_Port::C, 3 > PIN_17;
typedef struct NS::Pin<NS::_Port::C, 4 > PIN_18;
typedef struct NS::Pin<NS::_Port::C, 5 > PIN_19;
typedef struct NS::Pin<NS::_Port::B, 6 > PIN_20;
typedef struct NS::Pin<NS::_Port::B, 7 > PIN_21;

typedef struct NS::Pin<NS::_Port::C, 6 > PIN_DIP_1;
typedef PIN_0  PIN_DIP_2 ;
typedef PIN_1  PIN_DIP_3 ;
typedef PIN_2  PIN_DIP_4 ;
typedef PIN_3  PIN_DIP_5 ;
typedef PIN_4  PIN_DIP_6 ;
typedef PIN_20 PIN_DIP_9 ;
typedef PIN_21 PIN_DIP_10;
typedef PIN_5  PIN_DIP_11;
typedef PIN_6  PIN_DIP_12;
typedef PIN_7  PIN_DIP_13;
typedef PIN_8  PIN_DIP_14;
typedef PIN_9  PIN_DIP_15;
typedef PIN_10 PIN_DIP_16;
typedef PIN_11 PIN_DIP_17;
typedef PIN_12 PIN_DIP_18;
typedef PIN_13 PIN_DIP_19;
typedef PIN_14 PIN_DIP_23;
typedef PIN_15 PIN_DIP_24;
typedef PIN_16 PIN_DIP_25;
typedef PIN_17 PIN_DIP_26;
typedef PIN_18 PIN_DIP_27;
typedef PIN_19 PIN_DIP_28;

// ===========================
// USART
typedef PIN_DIP_2  PIN_RXD;
typedef PIN_DIP_3  PIN_TXD;
typedef PIN_DIP_6  PIN_XCK; // USART external clock

// Oscillators
// Chip clock Oscillator
typedef PIN_DIP_9  PIN_XTAL1;
typedef PIN_DIP_10 PIN_XTAL2;
// Timer Oscillator
typedef PIN_DIP_9  PIN_TOSC1;
typedef PIN_DIP_10 PIN_TOSC2;

// System clock
typedef PIN_DIP_14 PIN_CLK0;

// SPI
typedef PIN_DIP_17 PIN_MOSI;
typedef PIN_DIP_18 PIN_MISO;
typedef PIN_DIP_16 PIN_SS; // SS' !
typedef PIN_DIP_19 PIN_SCK;

// TWI
typedef PIN_DIP_28 PIN_SCL; // 2 wire Serial Interface Clock
typedef PIN_DIP_27 PIN_SDA; // 2 wire Serial Data

// Timer/Counter
// Timer0
typedef PIN_DIP_6  PIN_T0;   // Counter source
typedef PIN_DIP_11 PIN_OC0B; // Output compare match
typedef PIN_DIP_12 PIN_OC0A;

// Timer1
typedef PIN_DIP_11 PIN_T1;   // Counter source
typedef PIN_DIP_15 PIN_OC1A; // Output compare match
typedef PIN_DIP_16 PIN_OC1B;
typedef PIN_DIP_14 PIN_ICP1; // Input capture pin

// Timer2
typedef PIN_DIP_17 PIN_OC2A; // Output compare match
typedef PIN_DIP_5  PIN_OC2B;

// ADC
typedef PIN_DIP_23 PIN_ADC0;
typedef PIN_DIP_24 PIN_ADC1;
typedef PIN_DIP_25 PIN_ADC2;
typedef PIN_DIP_26 PIN_ADC3;
typedef PIN_DIP_27 PIN_ADC4;
typedef PIN_DIP_28 PIN_ADC5;

typedef PIN_DIP_12 PIN_AIN0; // Analog comparator negative input
typedef PIN_DIP_13 PIN_AIN1; // Analog comparator positive input

// External interrupt sources
typedef PIN_DIP_4  PIN_INT0;
typedef PIN_DIP_5  PIN_INT1;
// ===========================

typedef PIN_DIP_14 PIN_B0;
typedef PIN_DIP_15 PIN_B1;
typedef PIN_DIP_16 PIN_B2;
typedef PIN_DIP_17 PIN_B3;
typedef PIN_DIP_18 PIN_B4;
typedef PIN_DIP_19 PIN_B5;
typedef PIN_DIP_9  PIN_B6;
typedef PIN_DIP_10 PIN_B7;

typedef PIN_DIP_23 PIN_C0;
typedef PIN_DIP_24 PIN_C1;
typedef PIN_DIP_25 PIN_C2;
typedef PIN_DIP_26 PIN_C3;
typedef PIN_DIP_27 PIN_C4;
typedef PIN_DIP_28 PIN_C5;
typedef PIN_DIP_1  PIN_C6;

typedef PIN_DIP_2  PIN_D0;
typedef PIN_DIP_3  PIN_D1;
typedef PIN_DIP_4  PIN_D2;
typedef PIN_DIP_5  PIN_D3;
typedef PIN_DIP_6  PIN_D4;
typedef PIN_DIP_11 PIN_D5;
typedef PIN_DIP_12 PIN_D6;
typedef PIN_DIP_13 PIN_D7;


namespace NS {
  template <enum DataDirection Dd, class Io>
  uint8_t _set_or_get(uint8_t val);
  
  /**
   * @brief This class implements the cast operations from uint8_t and
   * some enums.
   * 
   * All functions redirect to _set_or_get().
   */
  template <enum _Port p, uint8_t b, enum _IOReg io>
  struct _Io {
    typedef _Io<p, b, io> _Io_t_;
    
    static const enum _Port port = p;
    static const uint8_t bit = b;
    static const enum _IOReg io_reg = io;
    
    _Io_t_& operator=(const uint8_t val) {
      _set_or_get<DataDirection::Write, _Io_t_>(val);
      return *this;
    }
    
    _Io_t_& operator=(const enum DataDirection val) {
      static_assert(io == _IOReg::DDRx, "Usage of DataDirection enum for incorrect register (not DDR).");
      _set_or_get<DataDirection::Write, _Io_t_>((uint8_t) val);
      return *this;
    }
    
    _Io_t_& operator=(const enum PullUp val) {
      static_assert(io == _IOReg::PORTx, "Usage of PullUp enum for incorrect register (not PORT).");
      _set_or_get<DataDirection::Write, _Io_t_>((uint8_t) val);
      return *this;
    }
    
    operator uint8_t() {
      return _set_or_get<DataDirection::Read, _Io_t_>(0);
    }
    
    operator PullUp() {
      static_assert(io == _IOReg::PORTx, "PullUp enum conversion requested for incorrect register (not PORT).");
      return _set_or_get<DataDirection::Read, _Io_t_>(0) == 0 ? PullUp::Off : PullUp::On;
    }
    
    operator DataDirection() {
      static_assert(io == _IOReg::DDRx, "DataDirection enum conversion requested for incorrect register (not DDR).");
      return _set_or_get<DataDirection::Read, _Io_t_>(0) == 0 ? DataDirection::Input : DataDirection::Output;
    }
  };
  
  
  /**
   * @brief Performs the bit "magic" to read a specifc bit from a register
   * or write only one bit to a register.
   * 
   * @tparam Dd (an enum ports::DataDirection) specifies the direction.
   * @param value is ignored if Dd is ports::DataDirection::Read.
   * 
   * Probably only useful internally.  Bit operations are not that hard
   * and we usually have the ports::DataDirection enum anyway.
   **/
  template <enum DataDirection Dd, class R>
  uint8_t _set_or_get_f(R& reg, uint8_t bit, uint8_t value) {
    if (Dd == DataDirection::Write) {
      if (value) reg |= _BV(bit);
      else reg &= ~_BV(bit);
      return 0;
    } else return reg & _BV(bit);
  }
  
  
  /**
   * @brief Redirects to _set_or_get_f() with the correct register.
   * 
   * @param value is ignored if Dd is ports::DataDirection::Read.
   * @tparam Io must have 2 static members: `io_reg` and `port`.
   * `typedef`s inside this file provide those static values.
   * 
   * If for instance Io::io_reg == DDRx and Io::port == _Port::B
   * _set_or_get_f() is called with `DDRB`.
   * 
   * Probably only useful internally.
   **/
  template <enum DataDirection Dd, class Io>
  uint8_t _set_or_get(uint8_t val) {
    static_assert(Io::io_reg == _IOReg::DDRx ||
                  Io::io_reg == _IOReg::PINx ||
                  Io::io_reg == _IOReg::PORTx,
                  "IO Register is unknown.  Only Ddr, Pin and Port are supported.");
    switch(Io::io_reg) {
      case _IOReg::DDRx:
        switch(Io::port) {
          case _Port::B:
            return _set_or_get_f<Dd>(DDRB, Io::bit, val);
          case _Port::C:
            return _set_or_get_f<Dd>(DDRC, Io::bit, val);
          case _Port::D:
            return _set_or_get_f<Dd>(DDRD, Io::bit, val);
        }
        break;
      case _IOReg::PORTx:
        switch(Io::port) {
          case _Port::B:
            return _set_or_get_f<Dd>(PORTB, Io::bit, val);
          case _Port::C:
            return _set_or_get_f<Dd>(PORTC, Io::bit, val);
          case _Port::D:
            return _set_or_get_f<Dd>(PORTD, Io::bit, val);
        }
        break;
      case _IOReg::PINx:
        switch(Io::port) {
          case _Port::B:
            return _set_or_get_f<Dd>(PINB, Io::bit, val);
          case _Port::C:
            return _set_or_get_f<Dd>(PINC, Io::bit, val);
          case _Port::D:
            return _set_or_get_f<Dd>(PIND, Io::bit, val);
        }
        break;
    }
    return 0;  // should never happen
  }
  
  
  /**
   * @brief For every (_Io, 'bit position') pair template arguments, uses
   * _Io::port to select #rB, #rC or #rD and _Io::bit to decide which bit
   * to read / write from this register.  If Dd is read returns this value
   * in the 'bit position'.  If Dd is write uses the bit in #val at
   * 'bit position' for the asignment.
   * 
   * Example: D7 == PIN_B3 and B7 == 2 and val == 0b11001100:
   * 
   * PIN_B3 has port B and pin 3.
   * 
   * If
   * 
   * * Dd is write: uses `val & _BV(2)`, which is 1 and assigns it to rB
   *   at position 3 (remember: port B, pin 3).
   * * Dd is read: reads from rB at position 3 and returns the read value
   *   at position 2 in the returned `uint8_t`.
   * 
   * If Dd is read, #val is not used.
   * 
   * Don't be intimated by the length of this function, a good compiler
   * will optimize this to a few simple instructions.
   * 
   * @tparam Dd data direction
   * @tparam Dx Dx::port and Dx::pin must be defined
   * @tparam Bx the bit position
   **/
  template <enum DataDirection Dd,
            class D7, uint8_t B7,
            class D6, uint8_t B6,
            class D5, uint8_t B5,
            class D4, uint8_t B4,
            class D3, uint8_t B3,
            class D2, uint8_t B2,
            class D1, uint8_t B1,
            class D0, uint8_t B0,
            typename R, typename V>
  inline static uint8_t _set_or_get_8_bits_regs(R& rB __attribute__ ((unused)),
                                                R& rC __attribute__ ((unused)),
                                                R& rD __attribute__ ((unused)),
                                                const V& val __attribute__ ((unused))) {
      uint8_t vB __attribute__ ((unused)) = 0;
      uint8_t vC __attribute__ ((unused)) = 0;
      uint8_t vD __attribute__ ((unused)) = 0;
      uint8_t maskB = 0;
      uint8_t maskC = 0;
      uint8_t maskD = 0;
      
      const uint8_t d0isB = D0::port == NS::_Port::B; const uint8_t d0isC = D0::port == NS::_Port::C;
      const uint8_t d1isB = D1::port == NS::_Port::B; const uint8_t d1isC = D1::port == NS::_Port::C;
      const uint8_t d2isB = D2::port == NS::_Port::B; const uint8_t d2isC = D2::port == NS::_Port::C;
      const uint8_t d3isB = D3::port == NS::_Port::B; const uint8_t d3isC = D3::port == NS::_Port::C;
      const uint8_t d4isB = D4::port == NS::_Port::B; const uint8_t d4isC = D4::port == NS::_Port::C;
      const uint8_t d5isB = D5::port == NS::_Port::B; const uint8_t d5isC = D5::port == NS::_Port::C;
      const uint8_t d6isB = D6::port == NS::_Port::B; const uint8_t d6isC = D6::port == NS::_Port::C;
      const uint8_t d7isB = D7::port == NS::_Port::B; const uint8_t d7isC = D7::port == NS::_Port::C;

      uint8_t& v0 = d0isB ? vB : (d0isC ? vC : vD);
      uint8_t& v1 = d1isB ? vB : (d1isC ? vC : vD);
      uint8_t& v2 = d2isB ? vB : (d2isC ? vC : vD);
      uint8_t& v3 = d3isB ? vB : (d3isC ? vC : vD);
      uint8_t& v4 = d4isB ? vB : (d4isC ? vC : vD);
      uint8_t& v5 = d5isB ? vB : (d5isC ? vC : vD);
      uint8_t& v6 = d6isB ? vB : (d6isC ? vC : vD);
      uint8_t& v7 = d7isB ? vB : (d7isC ? vC : vD);

      uint8_t& mask0 = d0isB ? maskB : (d0isC ? maskC : maskD);
      uint8_t& mask1 = d1isB ? maskB : (d1isC ? maskC : maskD);
      uint8_t& mask2 = d2isB ? maskB : (d2isC ? maskC : maskD);
      uint8_t& mask3 = d3isB ? maskB : (d3isC ? maskC : maskD);
      uint8_t& mask4 = d4isB ? maskB : (d4isC ? maskC : maskD);
      uint8_t& mask5 = d5isB ? maskB : (d5isC ? maskC : maskD);
      uint8_t& mask6 = d6isB ? maskB : (d6isC ? maskC : maskD);
      uint8_t& mask7 = d7isB ? maskB : (d7isC ? maskC : maskD);

      // avoid warning: left shift count >= width of type
      // even if not used.  (if compiled without optimization)
      if (D0::bit < 8 && B0 < 8) {
        const uint8_t shift0 = (D0::bit >= 8) ? 0 : D0::bit;
        mask0 |= 1 << shift0;
        if (Dd == NS::DataDirection::Write) v0 |= val & _BV(B0);
      }
      if (D1::bit < 8 && B1 < 8) {
        const uint8_t shift1 = (D1::bit >= 8) ? 0 : D1::bit;
        mask1 |= 1 << shift1;
        if (Dd == NS::DataDirection::Write) v1 |= val & _BV(B1);
      }
      if (D2::bit < 8 && B2 < 8) {
        const uint8_t shift2 = (D2::bit >= 8) ? 0 : D2::bit;
        mask2 |= 1 << shift2;
        if (Dd == NS::DataDirection::Write) v2 |= val & _BV(B2);
      }
      if (D3::bit < 8 && B3 < 8) {
        const uint8_t shift3 = (D3::bit >= 8) ? 0 : D3::bit;
        mask3 |= 1 << shift3;
        if (Dd == NS::DataDirection::Write) v3 |= val & _BV(B3);
      }
      if (D4::bit < 8 && B4 < 8) {
        const uint8_t shift4 = (D4::bit >= 8) ? 0 : D4::bit;
        mask4 |= 1 << shift4;
        if (Dd == NS::DataDirection::Write) v4 |= val & _BV(B4);
      }
      if (D5::bit < 8 && B5 < 8) {
        const uint8_t shift5 = (D5::bit >= 8) ? 0 : D5::bit;
        mask5 |= 1 << shift5;
        if (Dd == NS::DataDirection::Write) v5 |= val & _BV(B5);
      }
      if (D6::bit < 8 && B6 < 8) {
        const uint8_t shift6 = (D6::bit >= 8) ? 0 : D6::bit;
        mask6 |= 1 << shift6;
        if (Dd == NS::DataDirection::Write) v6 |= val & _BV(B6);
      }
      if (D7::bit < 8 && B7 < 8) {
        const uint8_t shift7 = (D7::bit >= 8) ? 0 : D7::bit;
        mask7 |= 1 << shift7;
        if (Dd == NS::DataDirection::Write) v7 |= val & _BV(B7);
      }
  
      if (maskB != 0) {
        if (Dd == NS::DataDirection::Write) rB = (rB & ~maskB) | vB;
        else vB = rB;
      }
      if (maskC != 0) {
        if (Dd == NS::DataDirection::Write) rC = (rC & ~maskC) | vC;
        else vC = rC;
      }
      if (maskD != 0) {
        if (Dd == NS::DataDirection::Write) rD = (rD & ~maskD) | vD;
        else vD = rD;
      }
      
      uint8_t ret = 0;
      if (Dd == NS::DataDirection::Read) {
        ret |= (v0 & _BV(D0::bit)) ? _BV(B0) : 0;
        ret |= (v1 & _BV(D1::bit)) ? _BV(B1) : 0;
        ret |= (v2 & _BV(D2::bit)) ? _BV(B2) : 0;
        ret |= (v3 & _BV(D3::bit)) ? _BV(B3) : 0;
        ret |= (v4 & _BV(D4::bit)) ? _BV(B4) : 0;
        ret |= (v5 & _BV(D5::bit)) ? _BV(B5) : 0;
        ret |= (v6 & _BV(D6::bit)) ? _BV(B6) : 0;
        ret |= (v7 & _BV(D7::bit)) ? _BV(B7) : 0;
      }
      return ret;
  }
  
  
/**
 * @brief Redirects to _set_or_get_8_bits_regs().
 * 
 * @tparam Reg may either be `DDRx`, `PINx` or `PORTx`.
 * 
 * * `DDRx`: redirects with `DDRB`, `DDRC` and `DDRD` as registers.
 * * `PORTx`: redirects with `PORTB`, `PORTC` and `PORTD` as registers.
 * * `PINx`: redirects with `PINB`, `PINC` and `PIND` as registers.
 * 
 * See other _set_or_get_8_bits_regs() for the meaning of all other
 * parameters.
 **/
  template <enum DataDirection Dd,
            enum _IOReg Reg,
            class D7, uint8_t B7,
            class D6, uint8_t B6,
            class D5, uint8_t B5,
            class D4, uint8_t B4,
            class D3, uint8_t B3,
            class D2, uint8_t B2,
            class D1, uint8_t B1,
            class D0, uint8_t B0,
            typename V, typename P>
  inline static uint8_t _set_or_get_8_bits(const V& val) {
    static_assert(Reg == _IOReg::DDRx ||
                  Reg == _IOReg::PINx ||
                  Reg == _IOReg::PORTx,
                  "IO Register is unknown.  Only _IOReg::DDRx, _IOReg::PINx and _IOReg::PORTx are supported.");
    switch (Reg) {
      case _IOReg::DDRx:
        return _set_or_get_8_bits_regs<Dd, D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0>(DDRB, DDRC, DDRD, val);
      case _IOReg::PORTx:
        return _set_or_get_8_bits_regs<Dd, D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0>(PORTB, PORTC, PORTD, val);
      case _IOReg::PINx:
        return _set_or_get_8_bits_regs<Dd, D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0>(PINB, PINC, PIND, val);
    }
    return 0; // should never happen
  }

  
  /**
   * @brief For every (_Io, 'bit position') pair template arguments, uses
   * _Io::port to select #rB, #rC or #rD and _Io::bit to decide which bit
   * to write on this register.  Uses the bit in val at 'bit position' as
   * value.
   * 
   * Example: D7 == PIN_B3 and B7 == 2 and val == 0b11001100:
   * 
   * PIN_B3 has port B and pin 3.
   * 
   * Uses `val & _BV(2)`, which is 1 and assigns it to rB
   *   at position 3 (remember: port B, pin 3).
   * 
   * @tparam Dx Dx::port and Dx::pin must be defined
   * @tparam Bx the bit position
   **/
  template <class D7, uint8_t B7,
  class D6 = PIN_UNUSED, uint8_t B6 = -1,
  class D5 = PIN_UNUSED, uint8_t B5 = -1,
  class D4 = PIN_UNUSED, uint8_t B4 = -1,
  class D3 = PIN_UNUSED, uint8_t B3 = -1,
  class D2 = PIN_UNUSED, uint8_t B2 = -1,
  class D1 = PIN_UNUSED, uint8_t B1 = -1,
  class D0 = PIN_UNUSED, uint8_t B0 = -1,
  typename R = uint8_t,
  typename V = uint8_t>
  inline static void set_8_bits(R& rB __attribute__ ((unused)),
                                R& rC __attribute__ ((unused)),
                                R& rD __attribute__ ((unused)),
                                const V& val) {
    NS::_set_or_get_8_bits<NS::DataDirection::Write,
    D7, B7,
    D6, B6,
    D5, B5,
    D4, B4,
    D3, B3,
    D2, B2,
    D1, B1,
    D0, B0>(rB, rC, rD, val);
  }


  /**
   * @brief For every (_Io, 'bit position') pair template arguments, uses
   * _Io::port to select #rB, #rC or #rD and _Io::bit to decide which bit
   * to read on this register.  Returns the read value in uint8_t at
   * 'bit position'.
   * 
   * Example: D7 == PIN_B3 and B7 == 2:
   * 
   * PIN_B3 has port B and pin 3.
   * 
   * Uses `rB & _BV(3)` and returns it at bit position 2.
   * 
   * @tparam Dx Dx::port and Dx::pin must be defined
   * @tparam Bx the bit position
   **/
  template <class D7, uint8_t B7,
  class D6 = PIN_UNUSED, uint8_t B6 = -1,
  class D5 = PIN_UNUSED, uint8_t B5 = -1,
  class D4 = PIN_UNUSED, uint8_t B4 = -1,
  class D3 = PIN_UNUSED, uint8_t B3 = -1,
  class D2 = PIN_UNUSED, uint8_t B2 = -1,
  class D1 = PIN_UNUSED, uint8_t B1 = -1,
  class D0 = PIN_UNUSED, uint8_t B0 = -1,
  typename R = uint8_t>
  inline static uint8_t get_8_bits(R& rB __attribute__ ((unused)),
                                   R& rC __attribute__ ((unused)),
                                   R& rD __attribute__ ((unused))) {
    return NS::_set_or_get_8_bits<NS::DataDirection::Read, D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0>(rB, rC, rD, 0);
  }


  /**
   * @brief Uses _Io::port to select #rB, #rC or #rD and _Io::bit to decide
   * which bit * to write on this register.
   * Assigns bit 7 from #val to the first _Io, bit 6 for the second,...
   * 
   * Example: D7 == PIN_B3 and val == 0b11001100:
   * 
   * PIN_B3 has port B and pin 3.
   * 
   * Uses `val & _BV(7)`, which is 1 and assigns it to rB
   *   at position 3 (remember: port B, pin 3).
   * 
   * @tparam Dx Dx::port and Dx::pin must be defined
   **/
  template <class D7,
  class D6 = PIN_UNUSED,
  class D5 = PIN_UNUSED,
  class D4 = PIN_UNUSED,
  class D3 = PIN_UNUSED,
  class D2 = PIN_UNUSED,
  class D1 = PIN_UNUSED,
  class D0 = PIN_UNUSED,
  typename R = uint8_t,
  typename V = uint8_t>
  inline static void set_8_byte(R& rB __attribute__ ((unused)),
                                R& rC __attribute__ ((unused)),
                                R& rD __attribute__ ((unused)),
                                const V& val) {
    set_8_bits<D7, 7, D6, 6, D5, 5, D4, 4, D3, 3, D2, 2, D1, 1, D0, 0>(rB, rC, rD, val);
  }


  /**
   * @brief Uses _Io::port to select #rB, #rC or #rD and _Io::bit to decide
   * which bit to read on this register.
   * Returns the read value in uint8_t at the corresponding bit position.
   * 
   * For the first _Io in bit 7, the second _Io in bit 6,...
   * 
   * Example: D7 == PIN_B3
   * 
   * PIN_B3 has port B and pin 3.
   * 
   * Uses `rB & _BV(3)` and returns it at bit position 7.
   * 
   * @tparam Dx Dx::port and Dx::pin must be defined
   **/
  template <class D7,
  class D6 = PIN_UNUSED,
  class D5 = PIN_UNUSED,
  class D4 = PIN_UNUSED,
  class D3 = PIN_UNUSED,
  class D2 = PIN_UNUSED,
  class D1 = PIN_UNUSED,
  class D0 = PIN_UNUSED,
  typename R = uint8_t>
  inline static uint8_t get_8_byte(R& rB __attribute__ ((unused)),
                                   R& rC __attribute__ ((unused)),
                                   R& rD __attribute__ ((unused))) {
    return get_8_bits<D7, 7, D6, 6, D5, 5, D4, 4, D3, 3, D2, 2, D1, 1, D0, 0>(rB, rC, rD, 0);
  }


  template <enum NS::_IOReg IOReg,
  class D7, uint8_t B7,
  class D6 = PIN_UNUSED, uint8_t B6 = -1,
  class D5 = PIN_UNUSED, uint8_t B5 = -1,
  class D4 = PIN_UNUSED, uint8_t B4 = -1,
  class D3 = PIN_UNUSED, uint8_t B3 = -1,
  class D2 = PIN_UNUSED, uint8_t B2 = -1,
  class D1 = PIN_UNUSED, uint8_t B1 = -1,
  class D0 = PIN_UNUSED, uint8_t B0 = -1,
  typename V = uint8_t>
  inline static void set_8_bits(const V& val) {
    NS::_set_or_get_8_bits<NS::DataDirection::Write, IOReg, D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0>(val);
  }
  
  template <enum NS::_IOReg IOReg,
  class D7, uint8_t B7,
  class D6 = PIN_UNUSED, uint8_t B6 = -1,
  class D5 = PIN_UNUSED, uint8_t B5 = -1,
  class D4 = PIN_UNUSED, uint8_t B4 = -1,
  class D3 = PIN_UNUSED, uint8_t B3 = -1,
  class D2 = PIN_UNUSED, uint8_t B2 = -1,
  class D1 = PIN_UNUSED, uint8_t B1 = -1,
  class D0 = PIN_UNUSED, uint8_t B0 = -1>
  inline static uint8_t get_8_bits() {
    return NS::_set_or_get_8_bits<NS::DataDirection::Read, IOReg, D7, B7, D6, B6, D5, B5, D4, B4, D3, B3, D2, B2, D1, B1, D0, B0>(0);
  }
  
  template <enum NS::_IOReg IOReg,
  class D7,
  class D6 = PIN_UNUSED,
  class D5 = PIN_UNUSED,
  class D4 = PIN_UNUSED,
  class D3 = PIN_UNUSED,
  class D2 = PIN_UNUSED,
  class D1 = PIN_UNUSED,
  class D0 = PIN_UNUSED>
  inline static uint8_t get_8_byte() {
    return get_8_bits<IOReg, D7, 7, D6, 6, D5, 5, D4, 4, D3, 3, D2, 2, D1, 1, D0, 0>();
  }
  
  template <enum NS::_IOReg IOReg,
  class D7,
  class D6 = PIN_UNUSED,
  class D5 = PIN_UNUSED,
  class D4 = PIN_UNUSED,
  class D3 = PIN_UNUSED,
  class D2 = PIN_UNUSED,
  class D1 = PIN_UNUSED,
  class D0 = PIN_UNUSED,
  typename V = uint8_t>
  inline static void set_8_byte(const V& val) {
    set_8_bits<IOReg, D7, 7, D6, 6, D5, 5, D4, 4, D3, 3, D2, 2, D1, 1, D0, 0>(val);
  }
  
  
  template <enum NS::_IOReg IOReg,
  uint8_t offset,
  class D3,
  class D2,
  class D1,
  class D0,
  typename V>
  inline static void set_4_nibble(V& val) {
    set_8_byte<IOReg, PIN_UNUSED, -1, PIN_UNUSED, -1, PIN_UNUSED, -1, PIN_UNUSED, -1, D3, 3 + offset, D2, 2 + offset, D1, 1 + offset, D0, 0 + offset>(val);
  }
}

#pragma pop_macro("NS")