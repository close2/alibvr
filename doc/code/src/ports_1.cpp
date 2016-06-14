#include "ports.h"

using namespace ports;

__attribute__ ((OS_main)) int main(void) {
  
  // «PORTS_ASSIGNMENT[^ *,]»
  // Possible IORegs: ports::IOReg::DDRx, ports::IOReg::PINx and ports::IOReg::PORTx

  //               #       *                   §       ~       ×       |         #* §~×|
  set_8_byte<DDRx, PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>(0b11110000);
  // Equivalent to: PIN_B1::DDR = 1; PIN_D2::DDR = 1; PIN_D4::DDR = 1;
  //                PIN_D5::DDR = 0; PIN_D3::DDR = 0; PIN_D7::DDR = 0; but efficiently:
  // DDRB = (DDRB & 0b11111101) | 0b00000010;
  // DDRD = (DDRD & 0b01000011) | 0b00010100;

  uint8_t x1 = get_8_byte<ports::IOReg::DDRx,
                          PIN_B1, PIN_D2, PIN_UNUSED, PIN_D4, PIN_D5, PIN_D3, PIN_D7>();
  // Unused pins are read as 0.
  // If called after the previous set_8_byte would return 0b11010000

  // Specify which bit should be assigned to which pin:
  //               |          ×          #          *          §          ~            #* §~×|
  set_8_bits<DDRx, PIN_D7, 1, PIN_D3, 2, PIN_B1, 7, PIN_D2, 6, PIN_D4, 4, PIN_D5, 3>(0b11110000);
  // Equivalent to previous set_8_byte instruction.

  // Specify which bit in returned value is which pin:
  uint8_t x2 = get_8_bits<ports::IOReg::DDRx,
                          PIN_D7, 1,
                          PIN_D3, 2,
                          PIN_B1, 7,
                          PIN_D2, 6,
                          PIN_D4, 4,
                          PIN_D5, 3>();
  // Equivalent to previous get_8_byte instruction.

  //            offset↓  #       *       §       ~           #*§~
  set_4_nibble<PORTx, 2, PIN_B1, PIN_D3, PIN_D2, PIN_D1>(0b00101100);
  // equivalent to: PIN_B1::PORT = 1; PIN_D3::PORT = 0; PIN_D2 = 1; PIN_D1 = 1; but efficiently.
  /*×*/
  
  for (;;);
  return 0;
}
