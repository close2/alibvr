#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_TIMER1_CAPT
#include "register_irq_task.h"
#define USE_IRQ_TIMER1_CAPT
#pragma pop_macro("IRQ_NAME")
