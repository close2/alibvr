# Tasks

This subsystem provides functionality to periodically run tasks /
functions.

The return value of a task specifies the requested pause until the
task should be called again.

Create tasks and register them.
```C++
+++TASK_WITH_ENABLE_F+++
```

Probably in your main periodically execute all tasks which are due
by calling `+++TASK_EXEC_COMMAND+++`:
```C++
for(;;) {
  +++TASK_EXEC_COMMAND+++;
}
```

As the task subsystem uses the system clock, we need to register all
irq tasks after our main:
```C++
+++TASK_REGISTER_IRQS+++
```




