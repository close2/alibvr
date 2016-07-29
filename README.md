INCOMPLETE
==========

Adc, ports access and irqs are starting to become somehow stable.

The API is still not fixed and very little has been actually tested.

Please come back in a few days / weeks / months.


Examples:
=========

Examples may be found in my other github-repository:
[alibvr-examples](https://www.github.com/close2/alibvr-examples)


Goal of this library:
=====================

This is library for the *atmega 328*.  This is the IC used in arduinos.

Every functionality of the atmega is exposed through a class with static
functions.  This library will never create objects!

Pins are assigned using `typedef`s instead of macros.

Using C++ templates it should be possible to have very few / no C-macros.
Also C++ templates are all "resolved" during compile time.  This means that
the produced code is very compact.


Doxygen documentation:
======================

This README only provides an overview.

Please see the [doxygen](http://close2.github.io/alibvr/doxygen/html/)
documentation for a detailed description.


Usage:
======

## ports.h

Contains the `typedef` definitions for all pins.

Whenever you need to access a port use a provided `typedef`.

`DDR`, `PORT` and `PIN` of every pin may be accessed through `::DDR`,
`::PORT` and `::PIN` on those `typedef`s.

Data direction and pull-up resistor may also be set using enums.  (Note that
the compiler is able to verify that a data direction enum is only used on
register DDR!  Macros would not be able to do so.)

### `typedef`s

The pin layout is compatible to arduino / arduino lite.

Prefix names in the following graph with `Pin_` to get
provided `typedef`s.  Example: `B5` becomes `Pin_B5`, `8` becomes `Pin_8`


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

### Setting or reading single DDR, PORT or PIN bits:

```C++
typedef PIN_C2 Pin_In;   // Give PIN_C2 a meaningful name.

// Use your favorite way of setting DDR of pin C2:
Pin_In::DDR = 0;         // PIN_C2::DDR = 0; would work as well.
Pin_In::DDR = ports::DataDirection::Read;  // Use the (safer) provided enum.
Pin_In::setDD(ports::DataDirection::Read); // Equivalent to the previous line.

Pin_In::setToInput(ports::PullUp::HighZ);  // Sets DDR and then PORT (pullup).

uint8_t i = Pin_In::PIN; // Read a value.
```

### Assigning / Reading multiple DDR, PORT or PIN bits in an optimized way:

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

### Assigning / Reading multiple bits from provided registers in an optimized way:

```C++
// For all get* / set* functions a version where you have to
// provide regB, regC and regD exists.  (Except set_4_nibble).
uint8_t rB = 0, rC = 0, rD = 0;
set_8_byte<PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>(rB, rC, rD, 0b11110000);
// rB becomes 0b00000010  (because of B1)
// rD becomes 0b00010100  (because of D2 and D4)
// D5, D3 and D7 would set corresponding bits in rD to 0.
```

### Enums:

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

### Example:

Turn on an Led on pin C2.

```C++
#include "ports.h"

typedef PIN_C2 Led;

__attribute__ ((OS_main)) int main(void) {
  Led::DDR = ports::DataDirection::Output;   // Put Led pin into output mode.
  Led::PORT = 1;  // Set Led pin output to high. (I.e. turn it on.)
  
  for (;;);
  return 0;
}
```

Note that the compiler is able to optimize the `Led::xxx` assignments
into simple `sbi` calls.


## Analog to digital conversion  ADC

When doing adc you have to specify
* an input
* a reference voltage
* a mode

### Input selection

In addition to input pins (see `PIN_ADC*` typedefs above)  
`Input::Temperature`, `Input::V1_1` and `Input::Gnd` are allowed inputs.

### Reference voltages

Possible references are defined in:
```C++
enum class Ref {
  ARef = 0b00,
  AVcc = 0b01,
  V1_1 = 0b11
};
```

### ADC modes

Possible modes are defined in:
```C++
enum class Mode {
  SingleConversion   = 0xFF,
  FreeRunning        = 0b000,
  TriggerAnalogComp  = 0b001,
  TriggerPCInt0      = 0b010,
  TriggerTimer0CompA = 0b011,
  TriggerTimer0OvF   = 0b100,
  TriggerTimer1CompB = 0b101,
  TriggerTimer1OvF   = 0b110,
  TriggerTimer1Capt  = 0b111
};
```

See the
[doxygen doc](http://close2.github.io/alibvr/doxygen/html/de/d26/namespaceadc.html#a8094fa55ea1a7729bb35c230163c0f8f)
for short descriptions of those enum values.


### Synchronous conversions

`adc_8bit()` or `adc_10bit()` by default start a conversion and
busywait for the conversion to finish.

```C++
MyAdc::init<PIN_ADC0>();
uint8_t adc1 = MyAdc::adc_8bit();

MyAdc2::init<PIN_ADC1>();
uint16_t adc2 = MyAdc2::adc_10bit();
```

A non-zero template argument to `adc_*bit()` will put the cpu into idle
sleep, which reduces noise and improves conversion results.

You have to register an ADC-irq handler if you want to use noise
reduction.  (The default handler would reset the cpu and `adc_*bit`
must enable irqs, so that the cpu leaves sleep mode after a
conversion.)

Your code will not compile (with a semi-useful error) if you don't
register a handler.

```C++
typedef Adc<Ref::V1_1> NoiseRedAdc; // use the default do_nothing irq-task
#define NEW_ADC NoiseRedAdc
#include REGISTER_ADC
// main { ...
NoiseRedAdc::init<PIN_ADC3>();
uint16_t adcNoiseRed = NoiseRedAdc::adc_10bit<1>();
// ... }  // end of main
#include REGISTER_IRQS
```

### Background conversions

### Using different inputs, reference voltages or modes

The Adc class itself allows you to specify input, reference voltage and
mode as template arguments.  It is however possible to override those
values when calling `init()`.

If you want to change the input, reference voltage or mode, simply call
`init()` with the new values as template arguments.

To make your intentions clearer in this case, you can use `Input::Unset`
as template argument for the Adc class, and only pass a real input when
calling `init()`.  This is however not necessary.  The generated code
will be either equal or very similar.

Using another Adc class with different template arguments is another
possibility.  Remember however, that the adc subsystem is shared.

Here one class (`typedef`) is used to sample two inputs.
```C++
typedef Adc<Ref::V1_1, Input::Unset> Adc1_1;
Adc1_1::init<PIN_ADC3>();
adc3 = Adc1_1::adc_8bit();
Adc1_1::init<PIN_ADC4>();
adc4 = Adc1_1::adc_8bit();
```

The same result using two classes:
```C++
typedef Adc<Ref::V1_1, PIN_ADC3> AdcAdc3;
typedef Adc<Ref::V1_1, PIN_ADC4> AdcAdc4;
AdcAdc3::init();
adc3 = AdcAdc3::adc_8bit();
AdcAdc4::init();
adc4 = AdcAdc4::adc_8bit();
```

Remember, the subsystem is shared:
```C++
AdcAdc3::init();
AdcAdc4::init();
// !!! THIS WON'T WORK !!!
// AdcAdc3::adc_8bit() will use PIN_ADC4 as input!
// AdcAdc4::init() switched the adc subsystem to input PIN_ADC4.
adc3 = AdcAdc3::adc_8bit();
adc4 = AdcAdc4::adc_8bit();
```

It is possible to register multiple `Task`s, but _all_ `Task`s will
be executed instead of only the one where the conversion was started!
