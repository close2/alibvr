# [ports.h](../src/ports.h)


## PORT PIN DDR

Those typedefs provide static access to PORT, PIN and DDR bits.  This is
done using a C++ cast operator and any overhead will be removed by the
compiler.


```C++
PIN_16::DDR = 1;
PIN_16::
```

**To read from a pin: `uint8_t in = PIN_16::PIN;`**

### `typedef`s

See [ports.h](src/ports.h) for all available typedefs.

The pin layout is compatible to arduino / arduino lite

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



