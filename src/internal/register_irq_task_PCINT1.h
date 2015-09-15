#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_PCINT1
#include "register_irq_task.h"
#define USE_IRQ_PCINT1
#pragma pop_macro("IRQ_NAME")
