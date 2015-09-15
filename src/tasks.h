#pragma once

// USAGE
//
// create tasks:
/*
TASK(AAA) {
  do_stuff;
  return ms_to_units(nextInMs);
}
*/
// then register it
/*
#define NEW_TASK AAA
#include REGISTER_TASK
*/
//
// nextInMs will be added to the _start_ of the last
// execution time!  This means if you want your function
// to execute every second and your function is long and
// takes 200ms you should return 1000 and not 800!
//
// name of task must be a valid class name (the macro creates a class)
//
// Then in your main:
/*
for (;;) {
  execTasks<uint16_t, TASK_LIST>();
}
*/
// uint16_t for 16bit clock precission
// TASK_LIST is a macro build by including REGISTER_NEW_TASK
// up to 20 tasks may be registered

#include "internal/task_list.h"
#include "clock.h"

#define TASK_LIST _task_list::TaskListEmpty

#define REGISTER_TASK "internal/register_task.h"

namespace _tasks {
  template <typename C, template <typename ...> class List, typename T, typename ...Tasks>
  static void execTasks(const C& clock, const C& previous);
  
  template <typename T, typename C>
  inline static void exec1Task(const C& clock, const C& previous) {
    static C next = 0;
    if (T::is_enabled()) {
      if (clock_reached(clock, previous, next)) {
        next = clock + T::run(clock);
      }
    } else {
      next = clock;
    }
  }
  
  template <typename C, typename T>
  inline static void execTasks(const C& clock, const C& previous, _task_list::TaskList<T>) {
    exec1Task<T>(clock, previous);
  }
  
  template <typename C, typename T, typename ...Tasks>
  inline static void execTasks(const C& clock, const C& previous, _task_list::TaskList<T, Tasks...>) {
    exec1Task<T>(clock, previous);
    execTasks(clock, previous, _task_list::TaskList<Tasks...>());
  }
}

template <typename time_type_t, typename T>
static inline void execTasks(time_type_t& clock, time_type_t& previous) {
  previous = clock;
  clock = get_clock<time_type_t>();
  _tasks::execTasks<>(clock, previous, T());
}

template <typename time_type_t, typename T>
static inline void execTasks() {
  static time_type_t clock = -1;
  static time_type_t previous = -1;
  execTasks<time_type_t, T>(clock, previous);
}

// if you change the name of a task here, adapt mini-tasks as well!
#define TASK(name) \
struct name { \
  static inline uint8_t is_enabled() { return 1; } \
  template<typename T> \
  static inline T run(const T& clock); \
}; \
template<typename T> \
T name::run(const T& clock)
