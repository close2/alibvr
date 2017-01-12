#pragma once

#include "internal/task_list.h"
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
 * http://stackoverflow.com/questions/18701798/building-and-accessing-a-list-of-types-at-compile-time/18704609#18704609
 * 
 * A tasks which always returns 0 will never be ignored by EXEC_TASKS
 * and called every time.  (Similar to void tasks.)  It will also
 * prevent the tasks subsystem to put the avr to sleep.
 **/

/**
 * @brief This macro is redefined when the REGISTER_TASK file
 * is included.
 * 
 * It is a define to a type.  Whenever a new task is added
 * this macro is redefined to "point" to a new template list
 * type.
 **/
#define TASK_LIST _task_list::TaskListEmpty

/**
 * @brief This header file should be included to register a new task.
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
   * This function is never implemented or executed.
   * ¤
   * 
   * This is the version for an empty task list and "returns"" the
   * smallest possible type: uint8_t.
   **/
  uint8_t _time_t_builder(const _task_list::TaskList<>&);
  
  /**
+++TIME_T_BUILDER_DOC[^,   * ]+++
   * 
   * This is the version when the task list is not empty.
   *
   * The return types of the task and the recursively called
   * _time_t_builder return type are compared and the bigger of the
   * 2 is used as return type.
   * 
   * @see _argType which turns an argument type to a return type which
   * is used with decltype.
   * 
   * @tparam Task The current (first) task of the tasklist.
   * @tparam Tasks The remaining tasks of the variadic task template
   * list.
   **/
  template<typename Task, typename... Tasks>
  auto _time_t_builder(const _task_list::TaskList<Task, Tasks...>&)
    -> decltype(type_comparisons::bigger_type(
                  decltype(_argType(Task::run))(),
                  decltype(_time_t_builder(_task_list::TaskList<Tasks...>()))()));
  
  /**
   * @brief The entry function for executing all tasks of a variable
   * template list.
   * 
   * This function determines the optimal type for the system clock
   * values and delegates executing the tasks to _execTasks.
   * 
   * @tparam Tasks The variadic template list containing the tasks
   * which should be executed.
   **/
  template <typename... Tasks>
  static inline decltype(_time_t_builder<Tasks...>(_task_list::TaskList<Tasks...>()))
  execTasks(const _task_list::TaskList<Tasks...>& taskList) {
    typedef decltype(_time_t_builder<Tasks...>(taskList)) time_t;
    return _execTasks<time_t, _task_list::TaskList<Tasks...>>();
  }
  
  /**
   * @brief A minimalistic wrapper for tasks
   * 
   * For variadic template lists functions would only be possible, if
   * they all had the same type.  This simple wrapper "converts"
   * tasks/functions to classes.
   * 
   * [Stackoverflow question about functions as tasks](http://stackoverflow.com/questions/40387760/functions-as-template-arguments)
   * 
   * @see TASK for a macro which automatically determines the types.
   * 
   * @tparam T The return type of the task.
   * @tparam T2 The argument type of the task (i.e. the system clock)
   * @tparam F The task/function.
   **/
  template <typename T, typename T2, T(*F)(T2)>
  struct TaskWrapper {
    static inline T run(T2 clock) {
      return F(clock);
    }
  };
}

#define EXEC_TASKS() ALIBVR_NAMESPACE_TASKS::execTasks(TASK_LIST())

/**
 * @brief Wraps a function/task with TaskWrapper.
 * 
 * @see TaskWrapper for an explaination why this is necessary.
 * 
 * The macro simply extracts the return type and the argument type and
 * passes them to TaskWrapper.
 * 
 * @param task The function/task which should be wrapped.
 **/
#define TASK(task) \
  ALIBVR_NAMESPACE_TASKS::TaskWrapper< \
    decltype(task(0)), \
    decltype(ALIBVR_NAMESPACE_TASKS::_argType(task)), \
    task>
