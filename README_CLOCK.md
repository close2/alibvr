# System clock

When `clock.h` is included `Timer0` is used as system clock.

This system clock is also used in some other parts of `alibvr`,
where `clock.h` is then automatically included.

The `Timer0` prescaler is set to clkIO/64.


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
+++CLOCK_CONVERSION+++
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
```

The simpler version will always use the current system clock.  By
getting the system clock once and passing it to the more complex
version, you may be a little faster.
