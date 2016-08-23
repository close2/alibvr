# System clock

When `clock.h` is included `Timer0` is used as system clock.

This system clock is also used in some other parts of this library,
where `clock.h` is automatically included.

The `Timer0` prescaler is set to clkIO/64.


## Usage

The static instance `clock::Clock` has cast operators to `uint8_t`,
`uint16_t` and `uint32_t`.

The returned value is the clkIO ticks divided by 64 since the avr has
been started.

```C++
+++CLOCK_BASIC+++
```


