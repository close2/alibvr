// FIXME write #include file which automatically changes frequency
#define F_CPU 1000000UL

#include "macros.h"
#include "tasks.h"
#include "lcd.h"
#include "pgmspace.h"
//#include "timer0.h"
#include "timer1.h"
//#include "timer2.h"
#include "servo.h"

#include "spi.h"

template <uint16_t PulseLength, typename P>
class Servo_Def {
public:
  static uint16_t get_pulse_length() {
    return PulseLength;
  }
  
  static void on() {
    SET_BIT(P, DDR, 1);
    SET_BIT(P, PORT, 1);
  }
  
  static void off() {
    SET_BIT(P, PORT, 0);
  }
};

typedef Servo_Def<1300, PIN_8> Servo1;
typedef Servo_Def<1800, PIN_6> Servo2;

template <typename P>
class Servo_Var {
public:
  static uint16_t get_pulse_length() {
    static uint16_t pulse_length = 700;
    static int8_t dir = 1;
    if (pulse_length < 500) dir = 1;
    if (pulse_length > 2000) dir = -1;
    pulse_length += dir;
    return pulse_length;
  }
  
  static void on() {
    SET_BIT(P, DDR, 1);
    SET_BIT(P, PORT, 1);
  }
  
  static void off() {
    SET_BIT(P, PORT, 0);
  }
};

typedef Servo_Var<PIN_7> Servo3;

typedef Servos<Servo1, Servo2, Servo3, _servo::ServoPause<1000> > ServoEngine;

//typedef Timer0<_timer::ClockPrescale, _timer::Disconnected, _timer::Disconnected, 0 /* SetDdrA */, 0 /* SetDdrB */, ServoEngine> ServoTimer;
//#define NEW_TIMER0 ServoTimer
//#include REGISTER_TIMER0

typedef Timer1<_timer::ClockSelect::Clock8, _timer::OutputMode::Disconnected, _timer::OutputMode::Disconnected, 0 /* SetDdrA */, 0 /* SetDdrB */, ServoEngine> ServoTimer1;
#define NEW_TIMER1 ServoTimer1
#include REGISTER_TIMER1

//typedef Timer2<_timer::ClockSelect::Clock32, _timer::Disconnected, _timer::Disconnected, 0 /* SetDdrA */, 0 /* SetDdrB */, ServoEngine> ServoTimer2;
//#define NEW_TIMER2 ServoTimer2
//#include REGISTER_TIMER2

typedef Lcd<PIN_17, PIN_16, PIN_15, PIN_14, PIN_19, PIN_18> LCD;


class DataProviderTask {
private:
  static uint8_t& n() {
    static uint8_t n = 0;
    return n;
  }
  
public:
  template <typename Spi>
  static _spi::ToBuffer rx_complete(uint8_t c) {
    n() = c;
    //Spi() << _spi::DataDirection::Write;
    return _spi::ToBuffer::No;
  }
  
  template <typename Spi>
  static void tx_complete() {
    //Spi() << _spi::DataDirection::EndFrame;
    //Spi() << _spi::DataDirection::Read;
  }
  
  template <typename Spi>
  static void tx_empty() {
    // called right after rx_complete switched to write mode
    // send n once then stop frame
    Spi() << (uint8_t)(n() + 0x10);
  }
};

typedef SpiMaster<_spi::ClockSelect::Clock128, 16, 3> SMaster;
typedef SpiSlave<0, 0, DataProviderTask, 0xFF> SSlave;
//typedef SpiSync<_spi::ClockSelect::Clock4, _spi::Mode::Master, _spi::PIN_SS> SpiMaster;
//typedef SpiSync<_spi::ClockSelect::Clock4, _spi::Mode::Slave, _spi::PIN_SS> SpiSlave;
//#define SPI_MASTER


#ifdef SPI_MASTER
#  define NEW_SPI SMaster
#  include REGISTER_SPI
#else
#  define NEW_SPI SSlave
#  include REGISTER_SPI
#endif

void example1Master() {
  LCD lcd;
  
  // initialize the Spi system
  SMaster::init();
  
  // intantiate the SMaster class.  Note that the compiler will optimize this
  // so that no object is created at all.
  auto spiMaster = SMaster();
  
  spiMaster << _spi::DataDirection::Write;
  spiMaster << "Let";
  spiMaster << "us exchange 3 by";
  spiMaster.flush();
  spiMaster << "tes.";
  spiMaster << _spi::DataDirection::ReadWrite;
  spiMaster << "123";
  uint8_t bytes[3];
  spiMaster >> bytes[0];
  spiMaster >> bytes[1];
  spiMaster >> bytes[2];
  lcd << 'i';
  if (spiMaster.has_error(_spi::ErrorCheck::FlushFirst)) {
    // not good, maybe just try again.
  } else {
    spiMaster << _spi::DataDirection::EndFrame;   
  }
  lcd << 'A';
  lcd << bytes[0];
  lcd << 'B';
  lcd << bytes[1];
  lcd << 'C';
  lcd << bytes[2];
  lcd << 'D';
  
  spiMaster.reset();
}

void example1Slave() {
  SSlave::init(_spi::DataDirection::ReadWrite); // set initial transmission char
}

void spiMaster() {
  example1Master();
}

void spiSlave() {
  example1Slave();
}


__attribute__ ((OS_main)) int main(void) {
#ifdef SPI_MASTER
  _delay_ms(200);
  LCD::init();
  spiMaster();
#else
  spiSlave();
#endif
  
  /*
  //const ServoTimer::size_t a = 0x02;
  //const ServoTimer::size_t b = 0x00;
  //ServoTimer::init(a, b);
  
  ServoTimer1::init(0xFFFF, 0xFFFF);
  
  //ServoTimer2::init(0xFF, 0xFF);
  
  LCD::init();
  
  LCD lcd;
  auto h = PgmDataPtr<>::ptr(PSTR("hello world!"));
  lcd << h;
  LCD::set_display_address(0x40);
  lcd << (uint32_t)987065043;
  
  for (;;) {}
  for (;;) {
    execTasks<uint16_t, TASK_LIST>();
  }
  */
  for (;;) {}
  return 0;
}

#define USE_ONLY_DEFINED_IRQS
#include REGISTER_IRQS
