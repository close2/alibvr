#ifndef FLAG_TO_SILENCE_COMPILER_WARNING // no guards!
// do not define this flag!

#pragma push_macro("NEW_TIMER2")

#define NEW_TIMER2 NEW_PWM2
#include "register_timer2.h"

#pragma pop_macro("NEW_TIMER2")

#endif
