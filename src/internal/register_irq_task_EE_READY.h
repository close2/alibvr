#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_EE_READY
#include "register_irq_task.h"
#define USE_IRQ_EE_READY
#pragma pop_macro("IRQ_NAME")
