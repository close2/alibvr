#include "ports.h"

__attribute__ ((OS_main)) int main(void) {
  // «PORTS_DDR_PORT_PIN[^ *,]»
  typedef PIN_C2 Pin_In;   // Give PIN_C2 a meaningful name.

  // Use your favorite way of setting DDR of pin C2:
  Pin_In::DDR = 0;         // PIN_C2::DDR = 0; would work as well.
  Pin_In::DDR = ports::DataDirection::Read;  // Use the (safer) provided enum.
  Pin_In::setDD(ports::DataDirection::Read); // Equivalent to the previous line.

  Pin_In::setToInput(ports::PullUp::HighZ);  // Sets DDR and then PORT (pullup).

  uint8_t i = Pin_In::PIN; // Read a value.
  /*¤*/
  
  // «PORTB[^ *,]»
  PORTB &= ~(_BV(2));
  /*¤*/
  
  // «PIN_B2[^ *,]»
  PIN_B2::DDR = 0;
  /*¤*/
  
  for (;;);
  return i;
}
