#define F_CPU 1000000
#include "clock.h"
#include "ports.h"

typedef PIN_C2 Led;

__attribute__ ((OS_main)) int main(void) {
  Led::DDR = ports::DataDirection::Output;   // Put Led pin into output mode.
  
  // «CLOCK_BASIC[^  ,]»
  uint16_t now = clock::Clock;
  uint16_t previous = now;
  uint16_t target = previous + clock::ms_to_units(5000);
  for (; !clock::clock_reached(now, previous, target); now = clock::Clock);
  /*¤*/
  
  Led::PORT = 1;  // Set Led pin output to high. (I.e. turn it on.)
  
  for (;;);
  return 0;
}

#include REGISTER_IRQS