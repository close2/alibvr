#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

// this include is not really necessary, as tasks.h already includes
// task_list.h
#include "task_list.h"

#pragma push_macro("TASK_NAME")

// include the generated task_names
// every time this file is included TASK_NAME is set to another value.
#include "task_names.h"

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

#pragma pop_macro("TASK_NAME")
#pragma pop_macro("TL")

#endif
