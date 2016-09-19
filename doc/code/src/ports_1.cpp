#include "ports.h"

using namespace ports;

int main(void) {
  
  // «PORTS_ASSIGNMENT[^  ,]»
  // Set the DDR bit on different pins:
  //   assign bit         7       6       5           4       3       2       1      :  76 4321
  set_8_byte<IOReg::DDRx, PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>(0b11110000);

  uint8_t x1 = get_8_byte<IOReg::DDRx,
                          PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>();
  // Unused pins are read as 0.
  // 0b11010000 if called after the previous set_8_byte.

  // Specify which bit should be assigned to which pin:
  set_8_bits<IOReg::DDRx,
             PIN_D7, 1, // extract bit 1 and assign to PIN_D7
             PIN_D3, 2, // extract bit 2 and assign to PIN_D3
             PIN_B1, 7,
             PIN_D2, 6,
             PIN_D4, 4,
             PIN_D5, 3>(0b11110000);
  // Equivalent to previous set_8_byte instruction.

  // Specify which bit in returned value is which pin:
  uint8_t x2 = get_8_bits<IOReg::DDRx,
                          PIN_D7, 1,
                          PIN_D3, 2,
                          PIN_B1, 7,
                          PIN_D2, 6,
                          PIN_D4, 4,
                          PIN_D5, 3>();
  // Equivalent to previous get_8_byte instruction.

  //                bit in value v       v       v       v         vvvv
  //                         3+3=6   2+3=5   1+3=4   0+3=3   ↓offs 6543
  set_4_nibble<IOReg::PORTx, PIN_B1, PIN_D3, PIN_D2, PIN_D1, 3>(0b00101100);
  /*¤*/
  
  if (x1 == 0) x1 = x2;
  for (;;);
  return 0;
}
