#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_TWI
#include "register_irq_task.h"
#define USE_IRQ_TWI
#pragma pop_macro("IRQ_NAME")
