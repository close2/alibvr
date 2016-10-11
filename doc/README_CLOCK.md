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
+++CLOCK_BASIC+++
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
+++CLOCK_DURATION+++
```

The simpler version will always use the current system clock.  By
getting the system clock once and passing it to the more complex
version, you may be a little faster.


### Calculate if a clock value has been reached.

These functions are only necessary because system clock values may
wrap.  Only when the clock types are big enough to never wrap
(`uint64_t`) can we be sure that earlier clock values are also smaller
clock values.

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
works fine: `+++CLOCK_REACHED_UINT16+++`.

```C++
+++CLOCK_REACHED1+++
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
+++CLOCK_REACHED2+++
```
