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

+++PORT_TYPEDEFS+++

Pins `VCC`, `GND`, `AREF` and `AVCC` are only for documentation
and don't provide `typedef`s.


## Setting or reading single DDR, PORT or PIN bits:

```C++
+++PORTS_DDR_PORT_PIN+++
```


## Assigning / Reading multiple DDR, PORT or PIN bits in an optimized way:

```C++
+++PORTS_ASSIGNMENT+++
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


## Examples:

Turn on an Led on pin C2.

```C++
+++LED_EX+++
```

Note that the compiler is able to optimize the `Led::xxx` assignments
into simple `sbi` calls.
