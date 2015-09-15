#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_ANALOG_COMP
#include "register_irq_task.h"
#define USE_IRQ_ANALOG_COMP
#pragma pop_macro("IRQ_NAME")
