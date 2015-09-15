#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_PCINT0
#include "register_irq_task.h"
#define USE_IRQ_PCINT0
#pragma pop_macro("IRQ_NAME")
