# Ports

Avr Atmegas access and control pins through registers. The registers
are 8 bit wide and up to 8 pins are therefore controlled with every
register.

- The DDR registers controls the data directions.
- The PORT registers the output or the pull up resistors.
- The PIN registers toggle the output or are used to read values from pins.


All pin selections in this library are done using `typedef`s.  See below
for an example.

`DDR`, `PORT` and `PIN` of every pin may be accessed through the static
fields `::DDR`, `::PORT` and `::PIN` on those `typedef`s.

Data direction and pull-up resistor may also be set using enums.  (Note that
the compiler is able to verify that a data direction enum is only used on
register DDR!  Macros would not be able to do so.)

The `ports` namespace also provides some helper functions to
efficiently assign multiple values.


## Doxygen documentation

The ports subsystem is documented in the:
[namespace ports](http://close2.github.io/alibvr/doxygen/html/d4/d49/namespaceports.html)


## `typedef`s

The pin layout is compatible to arduino / arduino lite.

Prefix names in the following graph with `PIN_` to get
provided `typedef`s.  Example: `B5` → `PIN_B5`, `8` → `PIN_8`


```
                                 ┏━u━┓
                     C6    DIP_1 ┃   ┃ DIP_28   19   C5   ADC5   SCL
RXD              0   D0    DIP_2 ┃   ┃ DIP_27   18   C4   ADC4   SDA
TXD              1   D1    DIP_3 ┃   ┃ DIP_26   17   C3   ADC3
        INT0     2   D2    DIP_4 ┃   ┃ DIP_25   16   C2   ADC2
OC2B    INT1     3   D3    DIP_5 ┃   ┃ DIP_24   15   C1   ADC1
XCK     T0       4   D4    DIP_6 ┃   ┃ DIP_23   14   C0   ADC0
                             VCC ┃   ┃ GND
                             GND ┃   ┃ AREF
XTAL1   TOSC1   20   B6    DIP_9 ┃   ┃ AVCC
XTAL2   TOSC2   21   B7   DIP_10 ┃   ┃ DIP_19   13   B5   SCK
T1      OC0B     5   D5   DIP_11 ┃   ┃ DIP_18   12   B4   MISO
AIN0    OC0A     6   D6   DIP_12 ┃   ┃ DIP_17   11   B3   MOSI   OC2A
AIN1             7   D7   DIP_13 ┃   ┃ DIP_16   10   B2   SS     OC1B
ICP1    CLK0     8   B0   DIP_14 ┃   ┃ DIP_15    9   B1          OC1A
                                 ┗━━━┛
```

Pins `VCC`, `GND`, `AREF` and `AVCC` are only for documentation
and don't provide `typedef`s.


## Setting or reading single DDR, PORT or PIN bits:

```C++
typedef PIN_C2 Pin_In;   // Give PIN_C2 a meaningful name.

// Use your favorite way of setting DDR of pin C2:
Pin_In::DDR = 0;         // PIN_C2::DDR = 0; would work as well.
Pin_In::DDR = ports::DataDirection::Read;  // Use the (safer) provided enum.
Pin_In::setDD(ports::DataDirection::Read); // Equivalent to the previous line.

Pin_In::setToInput(ports::PullUp::HighZ);  // Sets DDR and then PORT (pullup).

uint8_t i = Pin_In::PIN; // Read a value.
```


## Assigning / Reading multiple DDR, PORT or PIN bits in an optimized way:

```C++
// Set the DDR bit on different pins:
//   assign bit         7       6       5           4       3       2       1      :  76 4321
set_8_byte<IOReg::DDRx, PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>(0b11110000);

uint8_t x1 = get_8_byte<IOReg::DDRx,
                        PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>();
// Unused pins are read as 0.
// 0b11010000 if called after the previous set_8_byte.

// Specify which bit should be assigned to which pin:
set_8_bits<IOReg::DDRx,
           PIN_D7, 1, // extract bit 1 and assign to PIN_D7
           PIN_D3, 2, // extract bit 2 and assign to PIN_D3
           PIN_B1, 7,
           PIN_D2, 6,
           PIN_D4, 4,
           PIN_D5, 3>(0b11110000);
// Equivalent to previous set_8_byte instruction.

// Specify which bit in returned value is which pin:
uint8_t x2 = get_8_bits<IOReg::DDRx,
                        PIN_D7, 1,
                        PIN_D3, 2,
                        PIN_B1, 7,
                        PIN_D2, 6,
                        PIN_D4, 4,
                        PIN_D5, 3>();
// Equivalent to previous get_8_byte instruction.

//                bit in value v       v       v       v         vvvv
//                         3+3=6   2+3=5   1+3=4   0+3=3   ↓offs 6543
set_4_nibble<IOReg::PORTx, PIN_B1, PIN_D3, PIN_D2, PIN_D1, 3>(0b00101100);
```


## Assigning / Reading multiple bits from provided registers in an optimized way:

```C++
// For all get* / set* functions a version where you have to
// provide regB, regC and regD exists.  (Except set_4_nibble).
uint8_t rB = 0, rC = 0, rD = 0;
set_8_byte<PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>(rB, rC, rD, 0b11110000);
// rB becomes 0b00000010  (because of B1)
// rD becomes 0b00010100  (because of D2 and D4)
// D5, D3 and D7 would set corresponding bits in rD to 0.
```


## Enums:

```C++
enum class DataDirection {
  Read = 0,  Input = Read,   In = Read,
  Write = 1, Output = Write, Out = Write
};
```

```C++
enum class PullUp {
  Off = 0, HighZ = Off,
  On = 1
};
```

```C++
enum class IOReg {
  DDRx, PORTx, PINx
};
```


## Complete examples:

Turn on an Led on pin C2.

```C++
#include "ports.h"

typedef PIN_C2 Led;

int main(void) {
  Led::DDR = ports::DataDirection::Output;   // Put Led pin into output mode.
  Led::PORT = 1;  // Set Led pin output to high. (I.e. turn it on.)
  
  for (;;);
  return 0;
}
```

Note that the compiler is able to optimize the `Led::xxx` assignments
into simple `sbi` calls.
