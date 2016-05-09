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

```
                                     ┏━u━┓
            PCINT14            PC6  1┃   ┃28  PC5 (AI 5/*D19) PCINT13 ADC5 SCL
RXD         PCINT16      (D 0) PD0  2┃   ┃27  PC4 (AI 4/*D18) PCINT12 ADC4 SDA
TXD         PCINT17      (D 1) PD1  3┃   ┃26  PC3 (AI 3/*D17) PCINT11 ADC3
      INT0  PCINT18      (D 2) PD2  4┃   ┃25  PC2 (AI 2/*D16) PCINT10 ADC2
OC2B  INT1  PCINT19 PWM+ (D 3) PD3  5┃   ┃24  PC1 (AI 1/*D15) PCINT9  ADC1
XCK   T0    PCINT20      (D 4) PD4  6┃   ┃23  PC0 (AI 0/*D14) PCINT8  ADC0
                               VCC  7┃   ┃22  GND
                               GND  8┃   ┃21  AREF
XTAL1 TOSC1 PCINT6      *(D20) PB6  9┃   ┃20  AVCC
XTAL2 TOSC2 PCINT7      *(D21) PB7 10┃   ┃19  PB5 (D 13)      PCINT5  SCK
OC0B  T1    PCINT21 PWM+ (D 5) PD5 11┃   ┃18  PB4 (D 12)      PCINT4  MISO
OC0A  AIN0  PCINT22 PWM+ (D 6) PD6 12┃   ┃17  PB3 (D 11) PWM  PCINT3  MOSI OC2A
      AIN1  PCINT23      (D 7) PD7 13┃   ┃16  PB2 (D 10) PWM  PCINT2  SS   OC1B
CLKO  ICP1  PCINT0       (D 8) PB0 14┃   ┃15  PB1 (D 9)  PWM  PCINT1       OC1A
                                     ┗━━━┛
```

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



