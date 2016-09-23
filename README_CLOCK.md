# System clock

When `clock.h` is included `Timer0` is used as system clock.

This system clock is also used in some other parts of `alibvr`,
where `clock.h` is then automatically included.

The `Timer0` prescaler is set to clkIO/64 and global irqs are enabled.

See the [doxygen](http://close2.github.io/alibvr/doxygen/html/d9/d1f/namespaceclock.html)
documentation for more details.


## Usage

The static instance `clock::Clock` has cast operators to `uint8_t`,
`uint16_t` and `uint32_t`.

The returned values are the clkIO ticks divided by 64 since the avr has
been started and are named "units" in `alibvr`.

```C++
uint16_t previous = Clock;
while(!duration_passed(previous, ms_to_units<5000>()));
```


### Conversion to and from ms or µs

Functions to convert "units" to and from ms or µs are provided:
```C++
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
```

Versions accepting variable input are deprecated as they usually need
divisions.  As the avr doesn't provide hardware division the compiled
code is slow and big.

The dreprecation does not mean that I will remove the code in a future
version!

The template versions obviously calculate the conversions during
compile time.


### Calculate if duration has passed

To find out if a time duration has passed two functions are provided:

```C++
previous = Clock;
while(!duration_passed(previous, ms_to_units<5000>()));

// The simpler version above is equivalent to
// duration_passed(previous, Clock, ...);

previous = Clock;
for (;;) {
  uint16_t now = Clock;
  if (duration_passed(previous, now, ms_to_units<150>())) Led1::PORT = 1;
  if (duration_passed(previous, now, ms_to_units<220>())) Led2::PORT = 1;
  if (duration_passed(previous, now, ms_to_units<270>())) {
    Led1::PORT = 0;   // Led1 has been on for 270-150 = 120ms
    Led2::PORT = 0;   // Led2 has been on for 270-220 =  50ms
    // Start again: Led1 will turn on again in 150ms; Led2 in 220ms.
    previous = Clock;
  }
}
```

The simpler version will always use the current system clock.  By
getting the system clock once and passing it to the more complex
version, you may be a little faster.
