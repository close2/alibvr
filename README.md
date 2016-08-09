# INCOMPLETE

Adc, ports access and irqs are starting to become somehow stable.

The API is still not fixed and very little has been actually tested.

Please come back in a few days / weeks / months.


# Goal of this library:

This is library for the *atmega 328*.  This is the IC used in arduinos.

Instead of defining pins and settings with C-macros, everything is set
using C/C++ code.  This is mostly possible through template arguments.

For instance pin definitions are passed around as template arguments.

This class _never_ creates objects!  Everything is static and resolved
during compile time, making the generated code very compact and
efficient.  *This is not a typical C++ library!*

Ony very few features are implemented using C-macros.

I still hope that the described type-stack in
[b.atch.se](http://b.atch.se/) will at some point replace the
`#include` C-macro tricks, which are currently necessary to register
irq handlers.

This library is only tested with `gcc`!


# Documentation:

This README shows some simple examples.  Only parts of the
functionality of `alibvr` are covered.

Every subsystem has its own README_*.  I've tried to show the complete
functionality in those READMEs.

If you still need more detailed information use the
[doxygen](http://close2.github.io/alibvr/doxygen/html/) documentation
or simply read the comments in the code itself.

I find doxygen documentations hard to read and hope that most use cases
are covered through examples in the READMEs, but the doxygen comments
are _far_ more detailed.


# Subsystems:

[Ports](README_PORTS.md)

[Analog to digital conversion](README_ADC.md)


# Examples:

The following code snippets are extracted from `example*.cpp` files in
[doc/code/src](doc/code/src).

## [Turn on an led](doc/code/src/example_led.cpp)
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

## [Turn on an led if analog input is below a treshhold](doc/code/src/example_adc.cpp)
```C++
#define F_CPU 8000000
#include "ports.h"
#include "irqs.h"
#include "adc.h"

typedef PIN_C2 Led;
typedef PIN_ADC0 AnalogIn;

void f(const uint16_t& result) {
  Led::PORT = ((uint8_t)result) > 0x0F;
}

typedef adc::Adc<adc::Ref::V1_1, AnalogIn, adc::Mode::FreeRunning, f> MyAdc;
#define NEW_ADC MyAdc
#include REGISTER_ADC

__attribute__ ((OS_main)) int main(void) {
  Led::DDR = ports::DataDirection::Output;   // Put Led pin into output mode.
  Led::PORT = 1;  // Set Led pin output to high. (I.e. turn it on.)
  
  MyAdc::init();
  MyAdc::start_adc_8bit();
  
  
  for (;;);
  return 0;
}
#include REGISTER_IRQS
```