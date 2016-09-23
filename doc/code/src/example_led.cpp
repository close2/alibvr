#include "ports.h"

int main(void) {
//«LED_EX[^  ,]»
  typedef PIN_C2 Led;

  Led::DDR = ports::DataDirection::Output;   // Put Led pin into output mode.
  Led::PORT = 1;  // Set Led pin output to high. (I.e. turn it on.)
/*¤*/
  
  for (;;);
  return 0;
}
