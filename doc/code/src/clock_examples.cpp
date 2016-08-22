#define F_CPU 1000000
#include "clock.h"
#include "ports.h"

typedef PIN_C2 Led;

__attribute__ ((OS_main)) int main(void) {
  Led::DDR = ports::DataDirection::Output;   // Put Led pin into output mode.
  
  uint16_t previous = clock::Clock;
  uint16_t target = clock::ms_to_units(clock::units_to_ms(previous) + 5000);
  uint16_t now = previous;
  for (; !clock::clock_reached(now, previous, target); now = clock::Clock);
  
  Led::PORT = 1;  // Set Led pin output to high. (I.e. turn it on.)
  
  for (;;);
  return 0;
}

#include REGISTER_IRQS