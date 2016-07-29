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

Turn on an Led on pin C2.

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

### Input selection

In addition to input pins (see `PIN_ADC*` typedefs above)  
`Input::Temperature`, `Input::V1_1` and `Input::Gnd` are allowed inputs.

### Reference voltages

Possible references are defined in:
```C++
+++ADC_REFS+++
```

### ADC modes

Possible modes are defined in:
```C++
+++ADC_MODES+++
```

See the
[doxygen doc](http://close2.github.io/alibvr/doxygen/html/de/d26/namespaceadc.html#a8094fa55ea1a7729bb35c230163c0f8f)
for short descriptions of those enum values.


### Synchronous conversions

`adc_8bit()` or `adc_10bit()` by default start a conversion and
busywait for the conversion to finish.

```C++
+++ADC_SYNC+++
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
+++ADC_NOISE_RED1+++
// main { ...
+++ADC_NOISE_RED2+++
// ... }  // end of main
+++ADC_NOISE_RED3+++
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
+++ADC_2_INPUTS1+++
```

The same result using two classes:
```C++
+++ADC_2_INPUTS2+++
```

Remember, the subsystem is shared:
```C++
+++ADC_SHARED+++
```

It is possible to register multiple `Task`s, but _all_ `Task`s will
be executed instead of only the one where the conversion was started!
