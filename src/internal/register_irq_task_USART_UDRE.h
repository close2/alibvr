#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_USART_UDRE
#include "register_irq_task.h"
#define USE_IRQ_USART_UDRE
#pragma pop_macro("IRQ_NAME")
