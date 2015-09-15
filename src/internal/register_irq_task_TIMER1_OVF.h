#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_TIMER1_OVF
#include "register_irq_task.h"
#define USE_IRQ_TIMER1_OVF
#pragma pop_macro("IRQ_NAME")
