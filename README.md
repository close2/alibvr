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

Using C++ templates it should be possible to have very few / none C-macros.
Also C++ templates are all "resolved" during compile time.  This means that
the produced code is very compact.


Usage:
======

## ports.h

Contains the typedef definitions for all pins.

Whenever you need to access a port use a provided `typedef`.

`DDR`, `PORT` and `PIN` of every pin may be accessed through `::DDR`,
`::PORT` and `::PIN` on those `typedef`s.

Data direction and pull-up resistor may also be set using enums.  (Note that
the compiler is able to verify that a data direction enum is only used on
register DDR!  Macros would not be able to do so.)

### Setting or reading single DDR, PORT or PIN bits:

```C++
typedef PIN_C2 Pin_In;   // Give PIN_C2 a meaningful name.

// Use your favorite way of setting DDR of pin C2:
Pin_In::DDR = 0;         // PIN_C2::DDR = 0; would work as well.
Pin_In::DDR = _ports::DataDirection::Read;  // Use the (safer) provided enum.
Pin_In::setDd(_ports::DataDirection::Read); // Equivalent to the previous line.

Pin_In::setToInput(_ports::PullUp::HighZ);  // Sets DDR and then PORT (pullup).

uint8_t i = Pin_In::PIN; // Read a value from pin using automatic conversion.
```

### Assigning / Reading multiple DDR, PORT or PIN bits in an optimized way:

```C++
// Possible IORegs: DDRx, PINx and PORTx

//                              #       *                   §       ~       ¤       |         #* §~¤|
set_8_byte<_ports::IOReg::DDRx, PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>(0b11110000);
// Equivalent to: PIN_B1::DDR = 1; PIN_D2::DDR = 1; PIN_D4::DDR = 1;
//                PIN_D5::DDR = 0; PIN_D3::DDR = 0; PIN_D7::DDR = 0; but efficiently:
// DDRB = (DDRB & 0b11111101) | 0b00000010;
// DDRD = (DDRD & 0b01000011) | 0b00010100;

uint8_t x1 = get_8_byte<_ports::IOReg::DDRx,
                        PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>();
// Unused pins are set read as 0.  If called after the previous set_8_byte would
// return 0b11010000

// Specify which bit should be assigned to which pin:
//               |          ¤          #          *          §          ~            #* §~¤|
set_8_bits<DDRx, PIN_D7, 1, PIN_D3, 2, PIN_B1, 7, PIN_D2, 6, PIN_D4, 4, PIN_D5, 3>(0b11110000);
// Equivalent to previous set_8_byte instruction.

// Specify which bit in returned value is which pin:
uint8_t x2 = get_8_bits<_ports::IOReg::DDRx,
                        PIN_D7, 1,
                        PIN_D3, 2,
                        PIN_B1, 7,
                        PIN_D2, 6,
                        PIN_D4, 4,
                        PIN_D5, 3>();
// Equivalent to previous get_8_byte instruction.

//            offset↓  #       *       §       ~           #*§~
set_4_nibble<PORTx, 2, PIN_B1, PIN_D3, PIN_D2, PIN_D1>(0b00101100);
// equivalent to: PIN_B1::PORT = 1; PIN_D3::PORT = 0; PIN_D2 = 1; PIN_D1 = 1;
```

### Assigning / Reading multiple bits from provided registers in an optimized way:

```C++
// Version where you have to provide regB, regC and regD are also availabe:
// (except for set_4_nibble)
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


### `typedef`s

See [ports.h](src/ports.h) for all available typedefs.

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

### Example:

Turn an Led on pin C2 on.

```C++
#include "ports.h"

typedef PIN_C2 Led;
/* other possible ways to typedef the Led:
typedef PIN_DIP_25 Led;
typedef PIN_16 Led;
typedef PIN_ADC2 Led;
*/

__attribute__ ((OS_main)) int main(void) {
  // put Led pin into output mode.
  Led::DDR = 1;

  Led::PORT = 1;
  for (;;);
  return 0;
}
```

I've added a compiled (`-Os`) objdump of this code at the bottom.

Note that the compiler was able to optimize the `Led::xxx` assignments
into simple `sbi` calls.


## Analog to digital conversion  ADC

When doing adc you have to specify
* an input
* a reference voltage
* a mode


### Input selection

In addition to the provided `typedef`s for pins there are also
* `_adc::Input::Temperature`
* `_adc::Input::V1_1` and
* `_adc::Input::Gnd`


Assembler output of a simple program.
=====================================

This objdump demonstrates that the compiler is able to optimize
the C++ code into simple assembler calls.

The Led example above compiled with `-Os` will produce the following code:

```c-objdump
build/led.o:     file format elf32-avr
Disassembly of section .text:

00000000 <__vectors>:
   0:	0c 94 34 00 	jmp	0x68	; 0x68 <__ctors_end>
removed 24 similar irq table entries, which all point to <__bad_interrupt>
  64:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>

00000068 <__ctors_end>:
  68:	11 24       	eor	r1, r1
  6a:	1f be       	out	0x3f, r1	; 63
  6c:	cf ef       	ldi	r28, 0xFF	; 255
  6e:	d8 e0       	ldi	r29, 0x08	; 8
  70:	de bf       	out	0x3e, r29	; 62
  72:	cd bf       	out	0x3d, r28	; 61
  74:	0e 94 40 00 	call	0x80	; 0x80 <main>
  78:	0c 94 43 00 	jmp	0x86	; 0x86 <_exit>

0000007c <__bad_interrupt>:
  7c:	0c 94 00 00 	jmp	0	; 0x0 <__vectors>

00000080 <main>:
  80:	3a 9a       	sbi	0x07, 2	; 7
  82:	42 9a       	sbi	0x08, 2	; 8
  84:	ff cf       	rjmp	.-2      	; 0x84 <main+0x4>

00000086 <_exit>:
  86:	f8 94       	cli

00000088 <__stop_program>:
  88:	ff cf       	rjmp	.-2      	; 0x88 <__stop_program>

```


