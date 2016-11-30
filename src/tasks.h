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
  
  // use the argument type as return type.
  template <typename T1>
  T1 _argType(T1 (*)(T1));
  
  template <typename T1>
  T1 _argType(void (*)(T1));
  
  
  template <typename T, typename C>
  inline static C _exec1Task(C clock, C, void*) {
    typedef decltype(_argType(T::run)) arg_t;
    T::run((arg_t)clock);
    return std::numeric_limits<C>::max();
  }
  
  template <typename T, typename C, typename NotVoid>
  inline static C _exec1Task(C clock, C previous, const NotVoid*) {
    typedef decltype(_argType(T::run)) arg_t;
    static C next = 0;
    if (clock::clock_reached(clock, previous, next)) {
      next = clock + T::run((arg_t)clock);
    }
    return next;
  }
  
  template <typename C>
  inline static C _execTasks(C, C, _task_list::TaskList<>) {
    return std::numeric_limits<C>::max();
  }
  
  template <typename C, typename T, typename ...Tasks>
  inline static C _execTasks(C clock, C previous, _task_list::TaskList<T, Tasks...>) {
    typedef decltype(T::run(0)) ret_t;
    C next = _exec1Task<T>(clock, previous, (ret_t*)0);
    C other_nexts = _execTasks(clock, previous, _task_list::TaskList<Tasks...>());
    return next < other_nexts ? next : other_nexts;
  }
  
  template <typename C, typename TaskList>
  static inline C _execTasks() {
    static C clock = -1;
    C previous = clock;
    clock = (C) clock::Clock;
    return _execTasks(clock, previous, TaskList());
  }
  
  // If the taskList is empty return the smallest type possible: uint8_t
  uint8_t _time_t_builder(const _task_list::TaskList<>&);
  
  // Else let the compiler build the type by adding the return type of
  // the current task with the return type of the remaining Tasks.
  template<typename Task, typename... Tasks>
  auto _time_t_builder(const _task_list::TaskList<Task, Tasks...>&)
    -> decltype(decltype(_argType(Task::run))() +
                decltype(_time_t_builder(_task_list::TaskList<Tasks...>()))());
  
  
  template <typename... Tasks>
  static inline decltype(_time_t_builder<Tasks...>(_task_list::TaskList<Tasks...>()))execTasks(const _task_list::TaskList<Tasks...>& taskList) {
    typedef decltype(_time_t_builder<Tasks...>(taskList)) time_t;
    return _execTasks<time_t, _task_list::TaskList<Tasks...>>();
  }
  
  template <typename T, typename T2, T(*F)(T2)>
  struct TaskWrapper {
    static inline T run(T2 clock) {
      return F(clock);
    }
  };
}

#define EXEC_TASKS() ALIBVR_NAMESPACE_TASKS::execTasks(TASK_LIST())
#define TASK(task) ALIBVR_NAMESPACE_TASKS::TaskWrapper<decltype(task(0)), decltype(ALIBVR_NAMESPACE_TASKS::_argType(task)), task>
