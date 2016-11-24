#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

// this include is not really necessary, as tasks.h already includes
// task_list.h
#include "task_list.h"

#pragma push_macro("TASK_NAME")

// include the generated task_names
// every time this file is included TASK_NAME is set to another value.
#include "task_names.h"

#ifdef NEW_TASK8
# define NEW_TASK tasks::TaskWrapper8<NEW_TASK8>
#elif defined NEW_TASK16
# define NEW_TASK tasks::TaskWrapper16<NEW_TASK16>
#elif defined NEW_TASK32
# define NEW_TASK tasks::TaskWrapper32<NEW_TASK32>
#endif

namespace _tasks {
  typedef typename _task_list::concat<TASK_LIST, NEW_TASK>::task TASK_NAME;
}

#pragma push_macro("TL")

// include the generated task_list
// every time this file is included TASK_LIST and TL are set to another value.
#include "task_lists.h"

namespace _tasks {
  typedef TASK_NAME TL;
}

#undef NEW_TASK

#ifdef NEW_TASK8
#  undef NEW_TASK8
#endif

#if defined NEW_TASK16
#  undef NEW_TASK16
#endif

#if defined NEW_TASK32
#  undef NEW_TASK32
#endif

#pragma pop_macro("TASK_NAME")
#pragma pop_macro("TL")

#endif
