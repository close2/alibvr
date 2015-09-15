#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_WDT
#include "register_irq_task.h"
#define USE_IRQ_WDT
#pragma pop_macro("IRQ_NAME")
