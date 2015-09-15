#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_USART_RX
#include "register_irq_task.h"
#define USE_IRQ_USART_RX
#pragma pop_macro("IRQ_NAME")
