#define F_CPU 8000000

#include "tasks.h"
#include "ports.h"

typedef PIN_C1 Led1;
typedef PIN_C2 Led2;
typedef PIN_C3 LedButton;
typedef PIN_B1 Button;

void initPorts() {
  Led1::DDR = ports::DataDirection::Out;
  Led2::DDR = ports::DataDirection::Out;
  LedButton::DDR = ports::DataDirection::Out;
  Button::DDR = ports::DataDirection::Input;
}

uint16_t toggleLed1(uint16_t) {
  Led1::toggle();
  return clock::ms_to_units<300>();
}
#define NEW_TASK TASK(toggleLed1)
#include REGISTER_TASK

uint8_t toggleLed2(uint8_t) {
  Led2::toggle();
  return clock::us_to_units<700>();
}
#define NEW_TASK TASK(toggleLed2)
#include REGISTER_TASK

// «TASK_WITH_ENABLE_F»
uint8_t runTaskOnlyWhenButtonIsPressed() {
  return Button::PIN;
}
uint32_t toggleLedButton(uint32_t) {
  LedButton::toggle();
  return clock::ms_to_units<10000>();
}
#define NEW_TASK TASK(toggleLedButton, runTaskOnlyWhenButtonIsPressed)
#include REGISTER_TASK
/*¤*/

int main(void) {
  initPorts();
  // «TASK_EXEC[^  ,]»
  for(;;) {
    EXEC_TASKS();
  }
  /*¤*/
}

// «TASK_REGISTER_IRQS»
#include REGISTER_IRQS
/*¤*/
