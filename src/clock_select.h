#pragma once

namespace _clock_select {

  enum class ClockSelect {
    ClockOff = 0,
    Clock1 = 1,
    Clock2 = 2,
    Clock4 = 4,
    Clock8 = 8,
    Clock16 = 16,
    Clock32 = 32,
    Clock64 = 64,
    Clock128 = 128,
    Clock256 = 256,
    Clock1024 = 1024,
    ClockTFalling = 2, // external clock source
    ClockTRising = 3   // external clock source
  };

}