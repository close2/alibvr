# Tasks

This subsystem provides functionality to periodically run tasks /
functions.

The return value of a task specifies the requested pause until the
task should be called again.

Create tasks and register them.
```C++
uint8_t runTaskOnlyWhenButtonIsPressed() {
  return Button::PIN;
}
uint32_t toggleLedButton(uint32_t) {
  LedButton::toggle();
  return clock::ms_to_units<10000>();
}
#define NEW_TASK TASK(toggleLedButton, runTaskOnlyWhenButtonIsPressed)
#include REGISTER_TASK
```

Probably in your main periodically execute all tasks which are due:
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




