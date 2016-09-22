#define F_CPU 8000000
#include <util/delay.h>

#include "clock.h"
#include "ports.h"

using namespace clock;

typedef PIN_C2 Led;
typedef PIN_C3 WarnLed;
typedef PIN_D3 Led1;
typedef PIN_D4 Led2;

int main(void) {
  Led::DDR = ports::DataDirection::Output;   // Put Led pin into output mode.
  WarnLed::DDR = ports::DataDirection::Output;
  Led1::DDR = ports::DataDirection::Output;
  Led2::DDR = ports::DataDirection::Output;
  
  {
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    // «CLOCK_DIFF[^    ,]»
    uint8_t prev = Clock;
    _delay_ms(2);

    uint8_t now = Clock;
    uint8_t diff = now - prev;
    // Try to avoid the following line.
    uint8_t shouldBe2 = units_to_ms(diff);
  
    // Try not to use units_to_ms for variable values.
    // If possible convert a const value:
    uint8_t expected = ms_to_units<2>();
    /*¤*/
#   pragma GCC diagnostic pop
  
    // make gcc happy
    if (shouldBe2 != 2) {
      Led::PORT = 1;
    }
    if (expected != diff) {
      Led::PORT = 1;
    }
  }
  
  
  // «CLOCK_BASIC[^  ,]»
  uint16_t previous = Clock;
  while(!duration_passed(previous, ms_to_units<5000>()));
  /*¤*/
  
  // don't give the compiler a chance to optimize this away.
  uint8_t userMs = 8 + Led::PIN;
  uint8_t userUs = 8 + WarnLed::PIN;
  
  // «CLOCK_CONVERSION[^  ,]»
  // If the type is too small the compiler will warn us.
  const uint16_t durationInUnits = ms_to_units<300>();
  const uint8_t duration2InUnits = us_to_units<1500>();
  const uint8_t durationConvToMs = units_to_ms<50>();
  const uint32_t durationConvToUs = units_to_us<10000>();
  
  uint16_t clockStart = Clock;
  // Do something we want to measure.
  uint16_t clockEnd = Clock;
  auto measured = clockEnd - clockStart;
  
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  uint8_t measuredTimeInMs = units_to_ms(measured);
  uint8_t measuredTimeInUs = units_to_us(measured);
  
  // userMs and userUs might come from rs232.
  // If code size and calculation speed doesn't matter to you
  // using the deprecated function is perfectly fine.
  uint32_t userMsToUnits = ms_to_units(userMs); // userMs might come from rs232
  uint32_t userUsToUnits = us_to_units(userUs); // userUs might come from rs232
#pragma GCC diagnostic pop
  /*¤*/
  
  static_assert(duration2InUnits < durationInUnits, "1.5 ms should be smaller than 300ms");
  static_assert(durationConvToMs < durationConvToUs, "10000 units in µs must be > than 50 units in ms");
  if (measuredTimeInMs > 3) {
    // Operation took longer than 3 ms.
    Led::PORT = 1;
  }
  auto diff = measuredTimeInMs * 1000 - measuredTimeInUs;
  if (diff > 1000) {
    // The calculated difference is bigger than 1ms.
    WarnLed::PORT = 1;
  }
  
  // Wait the user entered ms:
  previous = Clock;
  while(!duration_passed(previous, userMsToUnits));
  // Wait the user entered us:
  previous = Clock;
  while(!duration_passed(previous, userUsToUnits));
  
  
  // «CLOCK_DURATION[^  ,]»
  previous = Clock;
  while(!duration_passed(previous, ms_to_units<5000>()));
  
  uint16_t duration1 = ms_to_units<150>();
  uint16_t duration2 = ms_to_units<220>();
  uint16_t duration3 = ms_to_units<270>();
  previous = Clock;
  for (;;) {
    uint16_t newClock = Clock;
    if (duration_passed(previous, newClock, duration1)) Led1::PORT = 1;
    if (duration_passed(previous, newClock, duration2)) Led2::PORT = 1;
    if (duration_passed(previous, newClock, duration3)) {
      Led1::PORT = 0;
      Led2::PORT = 0;
      previous = Clock;
    }
  }
  /*¤*/
  
  return 0;
}

#include REGISTER_IRQS
