#pragma once

#include "internal/task_list.h"
#include "internal/task_wrapper.h"
#include "clock.h"
#include "type_comparisons.h"


/**
 * @file
 * 
 * @brief This file provides an elegant way to register tasks and execute
 * them in your main loop.
 * 
 * There is no central place where tasks have to be listed.
 * 
 * The tasks subsystem may be consideres as a very rudimentary
 * OS-replacement.
 * 
 * AVRs commonly have to perform multiple tasks.
 * 
 * Simply write a task and register it.  It is even possible to register
 * tasks in header files.
 * 
 * A task is simply a function which accepts one argument: the current
 * system clock.
 * 
 * EXEC_TASKS (see below) will always call tasks (functions) which
 * return void.
 * 
 * Those void tasks should only be used for tasks which are executed
 * after an irq event.  (Think of those tasks as the second half of an
 * irq handler.)  This is important if you want the tasks subsystem to
 * put the avr to sleep.
 *
 * If a tasks returns a number, the number is considers to be time
 * units.  The task is then ignored until this many time units have
 * passed since the last _start_ of the task.
 * 
 * This means if you want your task/function to execute every second
 * and your function is long and takes 200ms you should return
 * ms_to_units<1000>() and not ms_to_units<800>()!
 * 
 * ```
+++ONE_SEC_TASK[^, * ]+++
 * ```
 * 
 * The mechanism how tasks are stored is an adapted version from:
 * [stackoverlow](http://stackoverflow.com/questions/18701798/building-and-accessing-a-list-of-types-at-compile-time/18704609#18704609)
 * 
 * A tasks which always returns 0 will never be ignored by EXEC_TASKS
 * and called every time.  (Similar to void tasks.)  It will also
 * prevent the tasks subsystem to put the avr to sleep.
 **/


/**
 * @brief This header file should be included to register a new task.
 * 
 * Every time this file is included TASK_LIST is redefined and a new
 * task (TaskWrapper) which must be defined as NEW_TASK is added to the
 * list.
 * 
 * ```
+++ONE_SEC_TASK[^, * ]+++
 * ```
 **/
#define REGISTER_TASK "internal/register_task.h"

#ifndef ALIBVR_NAMESPACE_TASKS
#  ifdef ALIBVR_NAMESPACE_PREFIX
#    define ALIBVR_NAMESPACE_TASKS ALIBVR_NAMESPACE_PREFIX ## tasks
#  else
#    define ALIBVR_NAMESPACE_TASKS tasks
#  endif
#endif


/**
 * @brief Task subsystem related code is (usually) in this namespace.
 * 
 * By default class related classes, enums,... are defined
 * inside the `tasks` namespace.  If this creates a name clash with your
 * code you may modify the namespace name by setting
 * ALIBVR_NAMESPACE_TASKS or ALIBVR_NAMESPACE_PREFIX.
 **/
namespace ALIBVR_NAMESPACE_TASKS {
  
  /**
   * @brief A small helper function, which extracts the _argument_
   * type from the function provided as template argument making it
   * available for decltype
   * 
   * For this version, both the argument and return type of the
   * provided function must be the same.
   **/
  template <typename T1>
  T1 _argType(T1 (*)(T1));
  
  /**
   * @brief A small helper function, which extracts the _argument_
   * type from the function provided as template argument making it
   * available for decltype
   * 
   * For this version, the return type of the provided function must
   * be void.
   **/
  template <typename T1>
  T1 _argType(void (*)(T1));
  
  
  /**
   * @brief Execute the task given as template argument.
   * 
   * This is the version for tasks which return void.
   * 
   * @tparam T The task which should be executed.
   * @tparam C The type of the clock value (uint8_t, uint16_t,...)
   * @param clock This clock value is passed to `T::run`
   **/
  template <typename T, typename C>
  inline static C _exec1Task(C clock, C, void*) {
    typedef decltype(_argType(T::run)) arg_t;
    T::run((arg_t)clock);
    return std::numeric_limits<C>::max();
  }
  
  /**
   * @brief Execute the task given as template argument.
   * 
   * This is the version for tasks which return time units.
   * If not enough time has expired the task is simply ignored and
   * this function does nothing.
   * 
   * @tparam T The task which should be executed.
   * @tparam C The type of the clock value (uint8_t, uint16_t,...)
   * @param clock This clock value is passed to `T::run`
   * @param previous This value is used when calling
   *   `clock::clock_reached` to find out if this task should be called
   *   or ignored.
   **/
  template <typename T, typename C, typename NotVoid>
  inline static C _exec1Task(C clock, C previous, const NotVoid*) {
    typedef decltype(_argType(T::run)) arg_t;
    static C next = 0;
    if (clock::clock_reached(clock, previous, next)) {
      next = clock + T::run((arg_t)clock);
    }
    return next;
  }
  
  /**
   * @brief Executes all tasks given as a variadic template argument list.
   * 
   * This is the end function, which is called when the task list is
   * empty.
   * 
   * @tparam C The type of the system clock values.
   **/
  template <typename C>
  inline static C _execTasks(C, C, _task_list::TaskList<>) {
    return std::numeric_limits<C>::max();
  }
  
  /**
   * @brief Executes all tasks given as a variadic template argument
   * list.
   * 
   * Tasks which return a number are ignored if not enough time has
   * passed.  @see _exec1Task.
   * The first task of the variadic template list is executed and this
   * function is recursively called with the remaining list.
   * 
   * @tparam C The type of the system clock values.
   * @tparam T The current task, which will be executed
   * @tparam Tasks The remaining task list.
   * @param clock The current system clock.
   * @param previous The system clock value, when _execTasks has been
   *   previously called.  This value is used when calling
   *   `clock::clock_reached`
   **/
  template <typename C, typename T, typename ...Tasks>
  inline static C _execTasks(C clock, C previous, _task_list::TaskList<T, Tasks...>) {
    typedef decltype(T::run(0)) ret_t;
    C next = _exec1Task<T>(clock, previous, (ret_t*)0);
    C other_nexts = _execTasks(clock, previous, _task_list::TaskList<Tasks...>());
    return next < other_nexts ? next : other_nexts;
  }
  
  /**
   * @brief Executes all tasks given as a variadic template argument
   * list.
   * 
   * This function remembers when it was called previously and passes
   * this value to the _execTasks function to which it delegates
   * actually executing the tasks.
   * 
   * @tparam C The type of the system clock values.
   * @tparam TaskList The variadic task template list.
   **/
  template <typename C, typename TaskList>
  static inline C _execTasks() {
    static C clock = std::numeric_limits<C>::max();
    C previous = clock;
    clock = (C) clock::Clock;
    return _execTasks(clock, previous, TaskList());
  }
  
  /**
   * «TIME_T_BUILDER_DOC[^   . ,]»
   * @brief Determines the smallest usable type necessary for system
   * clock values when exeuting tasks.
   * 
   * @see _argType which turns an argument type to a return type which
   * is used with decltype.
   * ¤
   **/
  template <typename ...> struct _time_t_builder;
  
  /**
+++TIME_T_BUILDER_DOC[^,   * ]+++
   * 
   * This is the version for the task list with only one remaining
   * task.  This might even be the TaskListEndMarker, in which case
   * uint8_t is returned.
   * 
   * @tparam List A task-list with only one remaining Task.
   * @tparam Task The last remaining task of the variadic template List.
   **/
  template <template <typename ...> class List, typename Task>
  struct _time_t_builder<List<Task>> {
    typedef decltype(_argType(Task::run)) type;
  };
  
  /**
+++TIME_T_BUILDER_DOC[^,   * ]+++
   * 
   * This is the version when the task list has more than one task.
   *
   * The return types of the task and the recursively called
   * _time_t_builder calculated type are compared and the bigger of the
   * 2 is used as type.
   * 
   * @tparam List A task-list with more than one Task.
   * @tparam Task The current (first) task of the tasklist.
   * @tparam Tasks The remaining tasks of the variadic task template
   * list.
   **/
  template <template <typename ...> class List, typename Task, typename ...Tasks>
  struct _time_t_builder<List<Task, Tasks...>> {
    typedef typename _time_t_builder<List<Tasks...>>::type type_parent;
    typedef decltype(type_comparisons::bigger_type(
                  decltype(_argType(Task::run))(),
                  type_parent())) type;
  };
  
  /**
   * @brief The entry function for executing all tasks of a variable
   * template list.
   * 
   * This function determines the optimal type for the system clock
   * values and delegates executing the tasks to _execTasks.
   * 
   * @tparam TaskList The variadic template list containing the tasks
   * which should be executed.
   **/
  template <typename TaskList>
  static inline typename _time_t_builder<TaskList>::type
  execTasks(TaskList) {
    typedef typename _time_t_builder<TaskList>::type time_t;
    return _execTasks<time_t, TaskList>();
  }
}

/**
 * @brief Calls execTasks with the TASK_LIST as argument.
 * 
 * Note that it is not possible to implement this as function, because
 * TASK_LIST will be redefined.  A function implementation would use
 * the first value of TASK_LIST, which is the empty list.
 * 
 * @see tasks::execTasks
 **/
#define EXEC_TASKS() ALIBVR_NAMESPACE_TASKS::execTasks(TASK_LIST())
