#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

#pragma push_macro("NEW_TASK")

#define NEW_TASK NEW_MODBUS
#include "register_task.h"

#undef NEW_MODBUS

#pragma pop_macro("NEW_TASK")

#endif
