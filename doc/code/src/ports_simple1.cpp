#include "ports.h"

using namespace ports;

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
  
  // «PIN_DDR_Ex1[^ *,]»
  PIN_C3::DDR = DataDirection::Input;
  /*¤*/
  
  // «PIN_DDR_Ex2[^ *,]»
  uint8_t currentDDR = PIN_C3::DDR;
  /*¤*/
  if (currentDDR == 0) currentDDR = 1;
  
  // «PIN_DDR_Ex3[^ *,]»
  enum DataDirection currentDDR2 = PIN_C3::DDR;
  /*¤*/
  if (currentDDR2 == DataDirection::Input) currentDDR2 = DataDirection::Output;
  
  // «PIN_PORT_Ex1[^ *,]»
  PIN_ADC4::PORT = 1;
  /*¤*/
  
  // «PIN_PORT_Ex2[^ *,]»
  uint8_t currentPort = PIN_ADC4::PORT;
  /*¤*/
  if (currentPort == 1) currentPort = 0;
  
  // «PIN_PORT_Ex3[^ *,]»
  enum PullUp currentPullUp = PIN_ADC4::PORT;
  /*¤*/
  if (currentPullUp == PullUp::HighZ) currentPullUp = PullUp::Off;
  
  // «PIN_PORT_Ex4[^ *,]»
  PIN_C3::PORT = PullUp::HighZ;
  /*¤*/
  
  // «PIN_PIN_Ex1[^ *,]»
  PIN_C3::PIN = 1; // equivalent to: PIN_C3::PORT = ! PIN_C3::PORT;
  /*¤*/
  
  // «PIN_PIN_Ex2[^ *,]»
  uint8_t input = PIN_C3::PIN;
  /*¤*/
  if (input == 0) input = 1;
  
  for (;;);
  return i;
}
