#define F_CPU 1000000
#include <util/delay.h>
#include "ports.h"
#include "clock.h"

typedef PIN_C4 Led;
typedef PIN_C2 FastLed;
typedef PIN_C3 SlowLed;

typedef PIN_C5 Button;

using namespace ports;
using namespace clock;

int main(void) {
  Led::DDR = DataDirection::Out;
  Led::PORT = 0;
  FastLed::DDR = DataDirection::Out;
  FastLed::PORT = 0;
  Button::DDR = DataDirection::In;
  
  _delay_ms(100);
  
  const uint16_t fastDuration = ms_to_units<100>();

  
  // «CLOCK_REACT[^  ,]»
  uint16_t startTimer = Clock;
  Led::PORT = 1;
  while (Button::PIN != 1);
  uint16_t stopTimer = Clock;
  if ((stopTimer - startTimer) < fastDuration) {
    FastLed::PORT = 1;
  }
  /*¤*/
  
  for(;;);
}

#include REGISTER_IRQS
