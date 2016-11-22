#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

#pragma push_macro("NEW_IRQ_TASK")
#pragma push_macro("IRQ_TASK_USAGE_COUNT")

#define NEW_IRQ_TASK NEW_TIMER1
#define IRQ_TASK_USAGE_COUNT 0
#include "register_irq_task_TIMER1_COMPA.h"

#define NEW_IRQ_TASK NEW_TIMER1
#undef IRQ_TASK_USAGE_COUNT
#define IRQ_TASK_USAGE_COUNT 1
#include "register_irq_task_TIMER1_COMPB.h"

#define NEW_IRQ_TASK NEW_TIMER1
#undef IRQ_TASK_USAGE_COUNT
#define IRQ_TASK_USAGE_COUNT 2
#include "register_irq_task_TIMER1_OVF.h"

#pragma pop_macro("NEW_IRQ_TASK")
#pragma pop_macro("IRQ_TASK_USAGE_COUNT")

#endif
