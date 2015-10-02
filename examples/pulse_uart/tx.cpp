// FIXME write #include file which automatically changes frequency
#define F_CPU 8000000UL

#include "clock.h"
#include "pulse_uart.h"

typedef PulseUartTx<PIN_DIP_12, ms_to_units<>(10), 2> PTx;

__attribute__ ((OS_main)) int main(void) {
  
  SET_BIT(PIN_DIP_12, DDR, 1);
  PTx tx;
  auto h = PgmDataPtr<>::ptr(PSTR("hello world!"));
  tx << h;
  tx << (uint32_t)987065043;
  
  //for (;;) {}
  return 0;
}

#define USE_ONLY_DEFINED_IRQS
#include REGISTER_IRQS
