#pragma push_macro("IRQ_NAME")
#define IRQ_NAME _irqs::Irq::IRQ_SPI_STC
#include "register_irq_task.h"
#define USE_IRQ_SPI_STC
#pragma pop_macro("IRQ_NAME")
