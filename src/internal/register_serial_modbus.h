#pragma push_macro("NAME_GEN")
#pragma push_macro("_NAME_GEN")
#define _NAME_GEN(name, class) _##name##_##class
#define NAME_GEN(name, class) _NAME_GEN(name, class)

#define NEW_UART NAME_GEN(NEW_SERIAL_MODBUS, UART)
typedef NEW_SERIAL_MODBUS ::_Uart NEW_UART;
#include "register_uart.h"


#define NEW_MODBUS NAME_GEN(NEW_SERIAL_MODBUS, MODBUS)
typedef NEW_SERIAL_MODBUS ::_Modbus NEW_MODBUS;
#include "register_modbus.h"

#undef NEW_SERIAL_MODBUS

#pragma pop_macro("NAME_GEN")
#pragma pop_macro("_NAME_GEN")