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
uint16_t now = clock::Clock;
uint16_t previous = now;
uint16_t target = previous + clock::ms_to_units(5000);
for (; !clock::clock_reached(now, previous, target); now = clock::Clock);
```


