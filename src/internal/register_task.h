#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

// These includes are not really necessary, as tasks.h already includes
// them.
#include "task_list.h"
#include "task_wrapper.h"

/**
 * @brief This macro is redefined when the REGISTER_TASK file
 * is included.
 * 
 * REGISTER_TASK is used like a typedef.  Whenever a new task is added
 * this macro is redefined to "point" to a new variadic template list
 * type.
 * 
 * The mechanism how tasks are stored is an adapted version from:
 * [stackoverlow](http://stackoverflow.com/questions/18701798/building-and-accessing-a-list-of-types-at-compile-time/18704609#18704609)
 * 
 * 
 * ```
+++ONE_SEC_TASK[^, * ]+++
 * ```
 **/
#ifndef TASK_LIST
#  define TASK_LIST _task_list::TaskListEmpty
#endif

#pragma push_macro("TASK_NAME")
#pragma push_macro("TASK")

// Include the generated task_names
// Every time this file is included TASK_NAME is set to another value.
#include "task_names.h"

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
#undef TASK
#define TASK(task) \
  _tasks::TaskWrapper< \
    decltype(task(0)), \
    decltype(ALIBVR_NAMESPACE_TASKS::_argType(task)), \
    task>

namespace _tasks {
  typedef typename _task_list::concat<TASK_LIST, TASK(NEW_TASK)>::task TASK_NAME;
}

#pragma push_macro("TL")

// include the generated task_list
// every time this file is included TASK_LIST and TL are set to another value.
#include "task_lists.h"

namespace _tasks {
  typedef TASK_NAME TL;
}

#pragma pop_macro("TL")

#undef NEW_TASK

#pragma pop_macro("TASK")
#pragma pop_macro("TASK_NAME")

#endif
