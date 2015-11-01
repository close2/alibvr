#pragma once

#include <avr/io.h>
#include "clock.h"
#include "tasks.h"

// USAGE
//
// create a task:
// TASK(xzy) {
//     do_stuff;
//     return ms_to_units(nextInMs);
// }
// and then register it either as mini-task A or B:
// MINI_TASK_A(xzy, 1);
// TODO MALTA NOTE  ↑ ENUM
// (true or false → start right away / don't automatically start mini-task)
//
// you might start / stop mini-tasks:
//   start_mini_task_{A,B}
//   stop_mini_task_{A,B}
//
// see tasks for more information about return value.
// note that the return value (the time difference) must be 8bit for mini-task!
// you can not wait for more than 16ms! (would be higher than 8bit when using 8MHz)

// n might be either A or B
#define _MINI_TASK(name, n, enabled) \
\
ISR(TIMER0_COMP##n##_vect) { \
  const uint8_t clock = get_clock<uint8_t>(); \
 \
  OCR0##n = clock + name::run(clock); \
} \
namespace _mini_tasks { \
  __attribute__((constructor)) \
  static void _maybe_enable_task_##n() { \
    if (enabled) { \
      start_mini_task_##n(); \
    }  \
  } \
}

static void start_mini_task_A() {
  OCR0A = TCNT0 + 1;
  TIMSK0 |= _BV(OCIE0A);
}
static void stop_mini_task_A() {
  TIMSK0 &= _BV(OCIE0A);
}
static void start_mini_task_B() {
  OCR0B = TCNT0 + 1;
  TIMSK0 |= _BV(OCIE0B);
}
static void stop_mini_task_B() {
  TIMSK0 &= _BV(OCIE0B);
}
  
#define MINI_TASK_A(name, enabled) _MINI_TASK(name, A, enabled)

#define MINI_TASK_B(name, enabled) _MINI_TASK(name, B, enabled)
