#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_PCINT2
#include "register_irq_task.h"
#define USE_IRQ_PCINT2
#pragma pop_macro("IRQ_NAME")
