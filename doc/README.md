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

+++PORT_TYPEDEFS+++

Pins `VCC`, `GND`, `AREF` and `AVCC` are only for documentation
and don't provide `typedef`s.

### Setting or reading single DDR, PORT or PIN bits:

```C++
+++PORTS_DDR_PORT_PIN+++
```

### Assigning / Reading multiple DDR, PORT or PIN bits in an optimized way:

```C++
+++PORTS_ASSIGNMENT+++
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

Turn an Led on pin C2 on.

```C++
+++LED_EX+++
```

Note that the compiler is able to optimize the `Led::xxx` assignments
into simple `sbi` calls.


## Analog to digital conversion  ADC

When doing adc you have to specify
* an input
* a reference voltage
* a mode

In addition to input pins (see ADC* typedefs above)  
`Temperature`, `V1_1` and `Gnd` are allowed inputs and defined
inside an inner `Input` namespace.

Possible references are defined in:
```C++
+++ADC_REFS+++
```

Possible modes are defined in:
```C++
+++ADC_MODES+++
```

See the doxygen doc for short summaries of those enum values.




### Input selection

In addition to the provided `typedef`s for pins there are also
* `_adc::Input::Temperature`
* `_adc::Input::V1_1` and
* `_adc::Input::Gnd`
