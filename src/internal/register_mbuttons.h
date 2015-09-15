#pragma push_macro("NEW_TASK")
#pragma push_macro("NEW_IRQ_TASK")
#pragma push_macro("IRQ_TASK_USAGE_COUNT")

#define NEW_TASK NEW_MBUTTONS
#include "register_task.h"

#pragma pop_macro("NEW_IRQ_TASK")
#pragma pop_macro("IRQ_TASK_USAGE_COUNT")

#pragma pop_macro("NEW_TASK")
