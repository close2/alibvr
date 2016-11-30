# Tasks

This subsystem provides functionality to periodically run tasks /
functions.

The return value of a task specifies the requested pause until the
task should be called again.

Create tasks and register them.
```C++
uint8_t toggleLed2(uint8_t) {
  Led2::toggle();
  return clock::us_to_units<700>();
}
#define NEW_TASK TASK(toggleLed2)
#include REGISTER_TASK
```

Probably in your main periodically execute all tasks which are due
by calling `EXEC_TASKS()`:
```C++
for(;;) {
  EXEC_TASKS();
}
```

As the task subsystem uses the system clock, we need to register all
irq tasks after our main:
```C++
#include REGISTER_IRQS
```




