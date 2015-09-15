#pragma push_macro("NEW_IRQ_TASK")
#pragma push_macro("IRQ_TASK_USAGE_COUNT")

#define NEW_IRQ_TASK NEW_TIMER2
#define IRQ_TASK_USAGE_COUNT 0
#include "register_irq_task_TIMER2_COMPA.h"

#define NEW_IRQ_TASK NEW_TIMER2
#undef IRQ_TASK_USAGE_COUNT
#define IRQ_TASK_USAGE_COUNT 1
#include "register_irq_task_TIMER2_COMPB.h"

#define NEW_IRQ_TASK NEW_TIMER2
#undef IRQ_TASK_USAGE_COUNT
#define IRQ_TASK_USAGE_COUNT 2
#include "register_irq_task_TIMER2_OVF.h"

#pragma pop_macro("NEW_IRQ_TASK")
#pragma pop_macro("IRQ_TASK_USAGE_COUNT")
