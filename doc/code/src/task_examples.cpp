#define F_CPU 8000000

#include "tasks.h"
#include "ports.h"

typedef PIN_C1 Led;
typedef PIN_C2 Led1;
typedef PIN_C3 Led2;
typedef PIN_C4 LedButton;
typedef PIN_B1 Button;

void initPorts() {
  Led::DDR = ports::DataDirection::Out;
  Led1::DDR = ports::DataDirection::Out;
  Led2::DDR = ports::DataDirection::Out;
  LedButton::DDR = ports::DataDirection::Out;
  Button::DDR = ports::DataDirection::Input;
}

static volatile uint8_t button_pressed = 0;

// «ONE_SEC_TASK»
uint32_t toggleLed(uint32_t) {
  Led::toggle();
  return clock::ms_to_units<1000>();
}
#define NEW_TASK TASK(toggleLed)
#include REGISTER_TASK
/*¤*/


uint16_t toggleLed1(uint16_t) {
  Led1::toggle();
  return clock::ms_to_units<300>();
}
#define NEW_TASK TASK(toggleLed1)
#include REGISTER_TASK

// «SIMPLE_TASK»
uint8_t toggleLed2(uint8_t) {
  Led2::toggle();
  return clock::us_to_units<700>();
}
#define NEW_TASK TASK(toggleLed2)
#include REGISTER_TASK
/*¤*/

// Think of toggleLedButton as the second part of an irq action.
// The irq handler would only set the button_pressed flag.
void toggleLedButton(uint32_t) {
  if (button_pressed) {
    LedButton::toggle();
    button_pressed = 0;
  }
}
#define NEW_TASK TASK(toggleLedButton)
#include REGISTER_TASK

int main(void) {
  initPorts();
  for(;;) {
    /* «TASK_EXEC_COMMAND» */EXEC_TASKS()/*¤*/;
  }
}

// «TASK_REGISTER_IRQS»
#include REGISTER_IRQS
/*¤*/
