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


### Calculate if a clock value has been reached.

These functions are only necessary because system clock values may
wrap.  Only when the clock types are big enough to never wrap
(`uint64_t`) can we be sure that earlier clock values are also smaller
clock values.  `uint64_t` versions are partially optimized and possibly
ignore arguments as wrapping can not occur.

These functions take wrapping into account.

The simpler version will always fetch the current system clock and use
the retrieved value for comparison.  Because of possible wrapping this
function will only work correctly if the time difference between the
call to the function and `target_clock` is below the values listed
below.

At 1 MHz and if `target_clock` is:
* `uint8_t`: ~8.5 ms
* `uint16_t`: ~2 s
* `uint32_t`: ~38 hours
* `uint64_t`: ~18'000'000 years

At 8 MHz and if `target_clock` is:
* `uint8_t`: ~1 ms
* `uint16_t`: ~0.25 s
* `uint32_t`: ~4.7 hours
* `uint64_t`: ~2'300'000 years

If for instance you want to wait 1.8 seconds at 1 MHz an `uint16_t`
works fine: `uint16_t target = (uint16_t) Clock + ms_to_units<1800>();`.

```C++
uint16_t now = Clock;
uint16_t target_clock = now + ms_to_units<1000>();
while (!clock_reached(target_clock)) {}
```

The more complete version of `clock_reached` expects two additional
values:
* the `previous_clock`.  Only when the system clock is no longer between
  `previous_clock` and `target_clock` has `target_clock` been reached.
* a `clock` value.  Usually the current system clock.  Getting the
  current system clock value is cheap but not free.
  Retrieving the system clock once and reusing it for different
  comparison can improve performance slightly.  (probably less than 10
  assembler instructions for every clock retrieval)

This function will only work correctly if the time difference between
`previous_clock` and `clock` is kept below the limits listed below:

At 1 MHz and if `target_clock` is:
* `uint8_t`: 16.384 ms
* `uint16_t`: 4.194304 s
* `uint32_t`: ~76 hours
* `uint64_t`: ~37'410'690 years

At 8 MHz and if `target_clock` is:
* `uint8_t`: 2.048 ms
* `uint16_t`: 0.524288 s
* `uint32_t`: ~9.5 hours
* `uint64_t`: ~4'654'596 years

```C++
uint16_t previous_clock = Clock;
uint16_t next_clock = previous_clock + ms_to_units<3000>();
while (!clock_reached((uint16_t) Clock, previous_clock, next_clock)) {}
```
