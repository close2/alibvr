#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

#pragma push_macro("NEW_TIMER0")

#define NEW_TIMER0 NEW_PWM0
#include "register_timer0.h"

#pragma pop_macro("NEW_TIMER0")

#endif
