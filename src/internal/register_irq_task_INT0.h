#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_INT0
#include "register_irq_task.h"
#define USE_IRQ_INT0
#pragma pop_macro("IRQ_NAME")

#endif
