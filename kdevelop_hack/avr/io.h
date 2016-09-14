#pragma once
#include </usr/avr/include/avr/io.h>

// Some random value to make llvm happy.
// avr-gcc will not use this.
#if defined __clang__ && defined FAKE_ADDR
#  undef _SFR_MEM_ADDR
#  define _SFR_MEM_ADDR(sfr) FAKE_ADDR 
#endif
