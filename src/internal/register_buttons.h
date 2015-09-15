#pragma push_macro("NEW_TASK")
#pragma push_macro("NEW_IRQ_TASK")
#pragma push_macro("IRQ_TASK_USAGE_COUNT")

#define NEW_IRQ_TASK NEW_BUTTONS
#define IRQ_TASK_USAGE_COUNT 0
#include "register_irq_task_PCINT0.h"

#define NEW_IRQ_TASK NEW_BUTTONS
#undef IRQ_TASK_USAGE_COUNT
#define IRQ_TASK_USAGE_COUNT 1
#include "register_irq_task_PCINT1.h"

#define NEW_IRQ_TASK NEW_BUTTONS
#undef IRQ_TASK_USAGE_COUNT
#define IRQ_TASK_USAGE_COUNT 2
#include "register_irq_task_PCINT2.h"

#define NEW_TASK NEW_BUTTONS
#include "register_task.h"

#pragma pop_macro("NEW_IRQ_TASK")
#pragma pop_macro("IRQ_TASK_USAGE_COUNT")

#pragma pop_macro("NEW_TASK")