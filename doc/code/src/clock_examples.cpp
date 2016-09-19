#define F_CPU 8000000
#include <util/delay.h>

#include "clock.h"
#include "ports.h"


typedef PIN_C2 Led;

int main(void) {
  Led::DDR = ports::DataDirection::Output;   // Put Led pin into output mode.
  
  {
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    // «CLOCK_DIFF[^    ,]»
    uint8_t prev = clock::Clock;
    _delay_ms(2);

    uint8_t now = clock::Clock;
    uint8_t diff = now - prev;
    // Try to avoid the following line.
    uint8_t shouldBe2 = clock::units_to_ms(diff);
  
    // Try not to use units_to_ms for variable values.
    // If possible convert a const value:
    uint8_t expected = clock::ms_to_units<2>();
    /*¤*/
#   pragma GCC diagnostic pop
  
    // make gcc happy
    if (shouldBe2 != 2) {
      Led::PORT = 1;
    }
    if (expected != diff) {
      Led::PORT = 1;
    }
  }
  
  
  // «CLOCK_BASIC[^  ,]»
  uint16_t previous = clock::Clock;
  while(!clock::duration_passed(previous, clock::ms_to_units<5000>()));
  /*¤*/
  
  Led::PORT = 1;  // Set Led pin output to high. (I.e. turn it on.)
  
  for (;;);
  return 0;
}

#include REGISTER_IRQS
