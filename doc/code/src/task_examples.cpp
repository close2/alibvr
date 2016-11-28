#define F_CPU 8000000

#include "tasks.h"
#include "ports.h"

typedef PIN_C1 Led1;
typedef PIN_C2 Led2;
typedef PIN_C3 Led3;
typedef PIN_B1 Button;

uint16_t toggleLed1(uint16_t) {
  Led1::toggle();
  return clock::ms_to_units<300>();
}
#define NEW_TASK16 toggleLed1
#include REGISTER_TASK

uint8_t toggleLed2(uint8_t) {
  Led2::toggle();
  return clock::us_to_units<700>();
}
#define NEW_TASK TASK(toggleLed2)
#include REGISTER_TASK

uint8_t isButtonPressed() {
  return Button::PIN;
}
uint32_t toggleLed3(uint32_t) {
  Led3::toggle();
  return clock::ms_to_units<10000>();
}
#define NEW_TASK TASK(toggleLed3, isButtonPressed)
#include REGISTER_TASK


int main(void) {
  for(;;) {
    EXEC_TASKS();
  }
  
}

#include REGISTER_IRQS
