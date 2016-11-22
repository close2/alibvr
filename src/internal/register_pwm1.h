#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

#pragma push_macro("NEW_TIMER1")

#define NEW_TIMER1 NEW_PWM1
#include "register_timer1.h"

#pragma pop_macro("NEW_TIMER1")

#endif
