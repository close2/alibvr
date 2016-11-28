#pragma once

#include "internal/task_list.h"
#include "clock.h"

  
//
// nextInMs will be added to the _start_ of the last
// call time!  This means if you want your function
// to execute every second and your function is long and
// takes 200ms you should return ms_to_units<1000>() and
// not ms_to_units<800>()!
//
// TASK_LIST is a macro build by including REGISTER_NEW_TASK
// up to 20 tasks may be registered

#define TASK_LIST _task_list::TaskListEmpty

#define REGISTER_TASK "internal/register_task.h"

#ifndef ALIBVR_NAMESPACE_TASKS
#  ifdef ALIBVR_NAMESPACE_PREFIX
#    define ALIBVR_NAMESPACE_TASKS ALIBVR_NAMESPACE_PREFIX ## tasks
#  else
#    define ALIBVR_NAMESPACE_TASKS tasks
#  endif
#endif


namespace ALIBVR_NAMESPACE_TASKS {
  
  template <typename T, typename C>
  inline static void _exec1Task(const C& clock, const C& previous) {
    static C next = 0;
    if (T::is_enabled()) {
      if (clock::clock_reached(clock, previous, next)) {
        next = clock + T::run(clock);
      }
    } else {
      next = clock;
    }
  }
  
  template <typename C>
  inline static void _execTasks(const C&, const C&, _task_list::TaskList<>) {
    return;
  }
  
  template <typename C, typename T, typename ...Tasks>
  inline static void _execTasks(const C& clock, const C& previous, _task_list::TaskList<T, Tasks...>) {
    _exec1Task<T>(clock, previous);
    _execTasks(clock, previous, _task_list::TaskList<Tasks...>());
  }
  
  template <typename C, typename TaskList>
  static inline void _execTasks(C& clock, C& previous) {
    previous = clock;
    clock = (C) clock::Clock;
    _execTasks<>(clock, previous, TaskList());
  }
  
  template <typename C, typename TaskList>
  static inline void _execTasks() {
    static C clock = -1;
    static C previous = -1;
    _execTasks(clock, previous, TaskList());
  }
  
  // If the taskList is empty return the smallest type possible: uint8_t
  uint8_t _time_t_builder(const _task_list::TaskList<>&);
  
  // Else let the compiler build the type by adding the return type of
  // the current task with the return type of the remaining Tasks.
  template<typename Task, typename... Tasks>
  auto _time_t_builder(const _task_list::TaskList<Task, Tasks...>&)
    -> decltype(decltype(Task::run(0))() +
                decltype(_time_t_builder(_task_list::TaskList<Tasks...>()))());
  
  
  template <typename... Tasks>
  static inline void execTasks(const _task_list::TaskList<Tasks...>& taskList) {
    _execTasks<decltype(_time_t_builder<Tasks...>(taskList)),
               _task_list::TaskList<Tasks...>>();
  }
  
  typedef uint8_t _is_enabled_f();
  uint8_t _always_enabled() { return 1; }
  
  template <typename T, T (*F)(T), _is_enabled_f EF = _always_enabled>
  struct TaskWrapper {
    static inline uint8_t is_enabled() { return EF(); }
    static inline T run(T clock) {
      return F(clock);
    }
  };
  
  
  template <uint8_t (*F)(uint8_t), _is_enabled_f EF = _always_enabled>
  using TaskWrapper8 = TaskWrapper<uint8_t, F, EF>;
  template <uint16_t (*F)(uint16_t), _is_enabled_f EF = _always_enabled>
  using TaskWrapper16 = TaskWrapper<uint16_t, F, EF>;
  template <uint32_t (*F)(uint32_t), _is_enabled_f EF = _always_enabled>
  using TaskWrapper32 = TaskWrapper<uint32_t, F, EF>;
}

#define EXEC_TASKS() ALIBVR_NAMESPACE_TASKS::execTasks(TASK_LIST())
#define TASK(task, ...) ALIBVR_NAMESPACE_TASKS::TaskWrapper<decltype(task(0)), task, ##__VA_ARGS__>
