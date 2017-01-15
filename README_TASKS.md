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
#define NEW_TASK toggleLed2
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

## Tasks

Every task must be a function which accepts the current time (in
units).  See [README_CLOCK](README_CLOCK).

The tasks subsystem differentiates between tasks which return void and
tasks which return a number (uint8_t, uint16_t, uint32_t or uint64_t).

EXEC_TASKS (see below) will always call tasks (functions) which return
void.

Those void tasks should only be used for tasks which are executed after
an irq event.  (Think of those tasks as the second half of an irq
handler.)  This is important if you want the tasks subsystem to put
the avr to sleep.

If a tasks returns a number, the number is considers time units.
The task is then by ignored EXEC_TASKS until this many time units
have passed since the last _start_ of the task.

This means if you want your task/function to execute every second
and your function is long and takes 200ms you should return
ms_to_units<1000>() and not ms_to_units<800>()!

```C++
uint32_t toggleLed(uint32_t) {
  Led::toggle();
  return clock::ms_to_units<1000>();
}
#define NEW_TASK toggleLed
#include REGISTER_TASK
```

A tasks which always returns 0 will never be ignored by EXEC_TASKS and
called every time.  (Similar to void tasks.)  It will also prevent the
tasks subsystem to put the avr to sleep.


## Execute tasks

