#define F_CPU 8000000

#include "ports.h"
#include "clock.h"
#include "tasks.h"

using namespace ports;
using namespace clock;

typedef PIN_B5 Led;

void init() {
  Led::PORT = 0;
  Led::setDD(DataDirection::Output);
}

inline uint16_t blinkLed(uint16_t) {
  static uint8_t state = 0;
  Led::toggle();
  state++;
  if (state == 8) {
    state = 0;
    return ms_to_units<500>();
  }
  return ms_to_units<200>();
}
#define NEW_TASK TASK(blinkLed)
#include REGISTER_TASK

int main() {
  init();
  for(;;) {
    EXEC_TASKS();
  }
}

#include REGISTER_IRQS
