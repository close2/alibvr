#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

// this include is not really necessary, as tasks.h already includes
// task_list.h
#include "task_list.h"

#pragma push_macro("TASKLIST_NAME_GEN")
#pragma push_macro("_TASKLIST_NAME_GEN")
#pragma push_macro("TASK_NAME")
#define _TASKLIST_NAME_GEN(newTask) TaskList##newTask
#define TASKLIST_NAME_GEN(newTask) _TASKLIST_NAME_GEN(newTask)

#ifdef TASK8
# define NEW_TASK tasks::TaskWrapper8<TASK8>
# define TASK_NAME TASK8
#elif defined TASK16
# define NEW_TASK tasks::TaskWrapper16<TASK16>
# define TASK_NAME TASK16
#elif defined TASK32
# define NEW_TASK tasks::TaskWrapper32<TASK32>
# define TASK_NAME TASK32
#else
# define TASK_NAME NEW_TASK
#endif

namespace _tasks {
  typedef typename _task_list::concat<TASK_LIST, NEW_TASK>::task TASKLIST_NAME_GEN(TASK_NAME);
}

#pragma push_macro("TL")
/*
 # !/bin/bash  *
 
 declare -i counter
 counter=$1
 
 FIRST=true
 
 declare -i i
 declare -i j
 declare -i k
 
 while [ $counter -gt 1 ]
 do
   j=$counter-1
   
   if [ $FIRST = "true" ]
   then
     printf '#if '
     FIRST=false
   else
     printf '#elif '
   fi
   echo "defined _TASK_LIST_COUNTER_$j"
   echo "#  define _TASK_LIST_COUNTER_$counter"
   echo "#  undef TASK_LIST"
   echo "#  define TL TaskList$counter"
   echo "#  define TASK_LIST _tasks::TaskList$counter"
   counter=$counter-1
 done

 echo "#else"
 echo "#  define _TASK_LIST_COUNTER_1"
 echo "#  undef TASK_LIST"
 echo "#  define TL TaskList1"
 echo "#  define TASK_LIST _tasks::TaskList1"
 echo "#endif"

 */

#if defined _TASK_LIST_COUNTER_19
#  define _TASK_LIST_COUNTER_20
#  undef TASK_LIST
#  define TL TaskList20
#  define TASK_LIST _tasks::TaskList20
#elif defined _TASK_LIST_COUNTER_18
#  define _TASK_LIST_COUNTER_19
#  undef TASK_LIST
#  define TL TaskList19
#  define TASK_LIST _tasks::TaskList19
#elif defined _TASK_LIST_COUNTER_17
#  define _TASK_LIST_COUNTER_18
#  undef TASK_LIST
#  define TL TaskList18
#  define TASK_LIST _tasks::TaskList18
#elif defined _TASK_LIST_COUNTER_16
#  define _TASK_LIST_COUNTER_17
#  undef TASK_LIST
#  define TL TaskList17
#  define TASK_LIST _tasks::TaskList17
#elif defined _TASK_LIST_COUNTER_15
#  define _TASK_LIST_COUNTER_16
#  undef TASK_LIST
#  define TL TaskList16
#  define TASK_LIST _tasks::TaskList16
#elif defined _TASK_LIST_COUNTER_14
#  define _TASK_LIST_COUNTER_15
#  undef TASK_LIST
#  define TL TaskList15
#  define TASK_LIST _tasks::TaskList15
#elif defined _TASK_LIST_COUNTER_13
#  define _TASK_LIST_COUNTER_14
#  undef TASK_LIST
#  define TL TaskList14
#  define TASK_LIST _tasks::TaskList14
#elif defined _TASK_LIST_COUNTER_12
#  define _TASK_LIST_COUNTER_13
#  undef TASK_LIST
#  define TL TaskList13
#  define TASK_LIST _tasks::TaskList13
#elif defined _TASK_LIST_COUNTER_11
#  define _TASK_LIST_COUNTER_12
#  undef TASK_LIST
#  define TL TaskList12
#  define TASK_LIST _tasks::TaskList12
#elif defined _TASK_LIST_COUNTER_10
#  define _TASK_LIST_COUNTER_11
#  undef TASK_LIST
#  define TL TaskList11
#  define TASK_LIST _tasks::TaskList11
#elif defined _TASK_LIST_COUNTER_9
#  define _TASK_LIST_COUNTER_10
#  undef TASK_LIST
#  define TL TaskList10
#  define TASK_LIST _tasks::TaskList10
#elif defined _TASK_LIST_COUNTER_8
#  define _TASK_LIST_COUNTER_9
#  undef TASK_LIST
#  define TL TaskList9
#  define TASK_LIST _tasks::TaskList9
#elif defined _TASK_LIST_COUNTER_7
#  define _TASK_LIST_COUNTER_8
#  undef TASK_LIST
#  define TL TaskList8
#  define TASK_LIST _tasks::TaskList8
#elif defined _TASK_LIST_COUNTER_6
#  define _TASK_LIST_COUNTER_7
#  undef TASK_LIST
#  define TL TaskList7
#  define TASK_LIST _tasks::TaskList7
#elif defined _TASK_LIST_COUNTER_5
#  define _TASK_LIST_COUNTER_6
#  undef TASK_LIST
#  define TL TaskList6
#  define TASK_LIST _tasks::TaskList6
#elif defined _TASK_LIST_COUNTER_4
#  define _TASK_LIST_COUNTER_5
#  undef TASK_LIST
#  define TL TaskList5
#  define TASK_LIST _tasks::TaskList5
#elif defined _TASK_LIST_COUNTER_3
#  define _TASK_LIST_COUNTER_4
#  undef TASK_LIST
#  define TL TaskList4
#  define TASK_LIST _tasks::TaskList4
#elif defined _TASK_LIST_COUNTER_2
#  define _TASK_LIST_COUNTER_3
#  undef TASK_LIST
#  define TL TaskList3
#  define TASK_LIST _tasks::TaskList3
#elif defined _TASK_LIST_COUNTER_1
#  define _TASK_LIST_COUNTER_2
#  undef TASK_LIST
#  define TL TaskList2
#  define TASK_LIST _tasks::TaskList2
#else
#  define _TASK_LIST_COUNTER_1
#  undef TASK_LIST
#  define TL TaskList1
#  define TASK_LIST _tasks::TaskList1
#endif

namespace _tasks {
  typedef TASKLIST_NAME_GEN(TASK_NAME) TL;
}

#undef NEW_TASK

#ifdef TASK8
#  undef TASK8
#endif

#if defined TASK16
#  undef TASK16
#endif

#if defined TASK32
#  undef TASK32
#endif

#pragma pop_macro("TASK_NAME")
#pragma pop_macro("TASKLIST_NAME_GEN")
#pragma pop_macro("_TASKLIST_NAME_GEN")
#pragma pop_macro("TL")

#endif
