#define F_CPU 8000000

#include "tasks.h"
#include "ports.h"

typedef PIN_C1 Led1;
typedef PIN_C3 LedButton;
typedef PIN_B1 Button;

void initPorts() {
  Led1::DDR = ports::DataDirection::Out;
  LedButton::DDR = ports::DataDirection::Out;
  Button::DDR = ports::DataDirection::Input;
}

// «TASK_EX»
uint16_t toggleLed1(uint16_t) {
  Led1::toggle();
  return clock::ms_to_units<300>();
}
#define NEW_TASK TASK(toggleLed1)
#include REGISTER_TASK

// Instead of fetching the current input from a pin,
// an irq-task could set a flag.  The current code
// would toggle continuously while the button is pressed.
void toggleLedButton(uint32_t) {
  if (Button::PIN) {
    LedButton::toggle();
  }
}
#define NEW_TASK TASK(toggleLedButton)
#include REGISTER_TASK

int main(void) {
  initPorts();
  for(;;) {
    EXEC_TASKS();
  }
}
#include REGISTER_IRQS
/*¤*/

