#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

#pragma push_macro("NEW_IRQ_TASK")

#define NEW_IRQ_TASK NEW_SPI
#include "register_irq_task_SPI_STC.h"

#pragma pop_macro("NEW_IRQ_TASK")

#endif
