#pragma once

namespace _transmission {
  
  enum class DataDirection {
    Read,       // expect a read.  If transmission is symmetric "ignores" the send part (and therefore send garbage, or the define default value)
    Write,      // expect a write. If transmission is symmetric "ignores" the received data
    ReadWrite,  // receive and send at the same time.
    ReadOnly,   // expect a read.  Don't send anything (dump data pushed into send buffer)
    WriteOnly,  // expect a write.  Dump everything received even if transmission is not symmetric
    EndFrame    // stop the current transmission (i.e. don't expect any more data)
  };
  
  class ClearErrors {};
  
  enum class ErrorCheck {
    FlushFirst,
    Now
  };
  
  enum class ToBuffer {
    No = 0,
    Yes = 1
  };
  
  template <class B>
  static void sleep_until_data_available() {
    const uint8_t sreg = SREG;

    cli();
    set_sleep_mode(SLEEP_MODE_IDLE);
    while (B::is_empty()) {
      sleep_enable();
      sei();
      sleep_cpu();
      sleep_disable();
      cli();
    }
    sei();

    SREG = sreg;
  }
  
}