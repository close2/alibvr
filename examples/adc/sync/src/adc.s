
adc.obj:     file format elf32-avr


Disassembly of section .text:

00000000 <__vectors>:
   0:	0c 94 34 00 	jmp	0x68	; 0x68 <__ctors_end>
   4:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
   8:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
   c:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  10:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  14:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  18:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  1c:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  20:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  24:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  28:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  2c:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  30:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  34:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  38:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  3c:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  40:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  44:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  48:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  4c:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  50:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  54:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  58:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  5c:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  60:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>
  64:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__bad_interrupt>

00000068 <__ctors_end>:
  68:	11 24       	eor	r1, r1
  6a:	1f be       	out	0x3f, r1	; 63
  6c:	cf ef       	ldi	r28, 0xFF	; 255
  6e:	d8 e0       	ldi	r29, 0x08	; 8
  70:	de bf       	out	0x3e, r29	; 62
  72:	cd bf       	out	0x3d, r28	; 61
  74:	0e 94 45 00 	call	0x8a	; 0x8a <main>
  78:	0c 94 87 00 	jmp	0x10e	; 0x10e <_exit>

0000007c <__bad_interrupt>:
  7c:	0c 94 00 00 	jmp	0	; 0x0 <__vectors>

00000080 <_ZN3AdcILN4_adc3RefE3EN6_ports3PinILNS2_4PortE1ELh5EEELNS0_4ModeE255ENS0_9DoNothingEE22busy_wait_adc_finishedEv>:
  static uint8_t is_adc_finished() {
    return (ADCSRA & _BV(ADSC)) == 0;
  }
  
  static void busy_wait_adc_finished() {
    loop_until_bit_is_clear(ADCSRA, ADSC);
  80:	80 91 7a 00 	lds	r24, 0x007A
  84:	86 fd       	sbrc	r24, 6
  86:	fc cf       	rjmp	.-8      	; 0x80 <_ZN3AdcILN4_adc3RefE3EN6_ports3PinILNS2_4PortE1ELh5EEELNS0_4ModeE255ENS0_9DoNothingEE22busy_wait_adc_finishedEv>
  }
  88:	08 95       	ret

0000008a <main>:
  };
  
  template <enum _Read_Write RW, class R>
  uint8_t _set_or_get_f(R& reg, uint8_t bit, uint8_t value) {
    if (RW == _Read_Write::Write) {
      if (value) reg |= _BV(bit);
  8a:	56 9a       	sbi	0x0a, 6	; 10
    static_assert(Input::port != _ports::Port::C || Input::bit != _adc::Input::Temperature::bit ||
                  Ref == _adc::Ref::V1_1,
                  "If input is temperature only V1_1 gives meaningful results.");

    uint8_t source = Input::bit << MUX0;
    ADMUX = ((uint8_t) Ref) << REFS0 // set ref
  8c:	d4 ec       	ldi	r29, 0xC4	; 196
    if (Mode != _adc::Mode::SingleConversion) {
      ADCSRB |= (uint8_t) Mode << ADTS0;
      ADCSRA |= _BV(ADATE);
    }
    // BALI Note: check if ADSC is high here
    ADCSRA = _BV(ADEN) // turn ADC power on
  8e:	c6 e8       	ldi	r28, 0x86	; 134
    static_assert(Input::port != _ports::Port::C || Input::bit != _adc::Input::Temperature::bit ||
                  Ref == _adc::Ref::V1_1,
                  "If input is temperature only V1_1 gives meaningful results.");

    uint8_t source = Input::bit << MUX0;
    ADMUX = ((uint8_t) Ref) << REFS0 // set ref
  90:	15 ec       	ldi	r17, 0xC5	; 197
  // the first adc is not guaranteed to have a meaningful value
  template <typename Input = DefaultInput,
            _adc::Ref Ref = DefaultRef,
            _adc::Mode Mode = DefaultMode>
  static void init() {
    PRR &= ~(_BV(PRADC)); // disable Power Reduction ADC
  92:	80 91 64 00 	lds	r24, 0x0064
  96:	8e 7f       	andi	r24, 0xFE	; 254
  98:	80 93 64 00 	sts	0x0064, r24
    static_assert(Input::port != _ports::Port::C || Input::bit != _adc::Input::Temperature::bit ||
                  Ref == _adc::Ref::V1_1,
                  "If input is temperature only V1_1 gives meaningful results.");

    uint8_t source = Input::bit << MUX0;
    ADMUX = ((uint8_t) Ref) << REFS0 // set ref
  9c:	d0 93 7c 00 	sts	0x007C, r29
    if (Mode != _adc::Mode::SingleConversion) {
      ADCSRB |= (uint8_t) Mode << ADTS0;
      ADCSRA |= _BV(ADATE);
    }
    // BALI Note: check if ADSC is high here
    ADCSRA = _BV(ADEN) // turn ADC power on
  a0:	c0 93 7a 00 	sts	0x007A, r28
    uint16_t res = ADCL;
    return res | ADCH << 8;
  }
  
  static uint8_t adc_8bit(uint8_t goto_sleep_for_noise_reduction = 0) {
    busy_wait_adc_finished();
  a4:	0e 94 40 00 	call	0x80	; 0x80 <_ZN3AdcILN4_adc3RefE3EN6_ports3PinILNS2_4PortE1ELh5EEELNS0_4ModeE255ENS0_9DoNothingEE22busy_wait_adc_finishedEv>
    ADMUX |= _BV(ADLAR); // 8bit → left adjusted
  a8:	80 91 7c 00 	lds	r24, 0x007C
  ac:	80 62       	ori	r24, 0x20	; 32
  ae:	80 93 7c 00 	sts	0x007C, r24
    
      ADCSRA |= _BV(ADSC);
      sleep_mode();
    } else {
      // start conversion:
      ADCSRA |= _BV(ADSC);
  b2:	80 91 7a 00 	lds	r24, 0x007A
  b6:	80 64       	ori	r24, 0x40	; 64
  b8:	80 93 7a 00 	sts	0x007A, r24
    // start conversion:
    _start_adc(goto_sleep_for_noise_reduction);

    // leave busy_wait outside else branch, because sleep for noise reduction
    // might wake up because of another irq.
    busy_wait_adc_finished();
  bc:	0e 94 40 00 	call	0x80	; 0x80 <_ZN3AdcILN4_adc3RefE3EN6_ports3PinILNS2_4PortE1ELh5EEELNS0_4ModeE255ENS0_9DoNothingEE22busy_wait_adc_finishedEv>
  static void busy_wait_adc_finished() {
    loop_until_bit_is_clear(ADCSRA, ADSC);
  }
  
  static uint8_t get_adc_8bit_result() {
    return ADCH; // left adjusted → the 8 most significant bits are in ADCH!
  c0:	00 91 79 00 	lds	r16, 0x0079
  // the first adc is not guaranteed to have a meaningful value
  template <typename Input = DefaultInput,
            _adc::Ref Ref = DefaultRef,
            _adc::Mode Mode = DefaultMode>
  static void init() {
    PRR &= ~(_BV(PRADC)); // disable Power Reduction ADC
  c4:	80 91 64 00 	lds	r24, 0x0064
  c8:	8e 7f       	andi	r24, 0xFE	; 254
  ca:	80 93 64 00 	sts	0x0064, r24
    static_assert(Input::port != _ports::Port::C || Input::bit != _adc::Input::Temperature::bit ||
                  Ref == _adc::Ref::V1_1,
                  "If input is temperature only V1_1 gives meaningful results.");

    uint8_t source = Input::bit << MUX0;
    ADMUX = ((uint8_t) Ref) << REFS0 // set ref
  ce:	10 93 7c 00 	sts	0x007C, r17
    if (Mode != _adc::Mode::SingleConversion) {
      ADCSRB |= (uint8_t) Mode << ADTS0;
      ADCSRA |= _BV(ADATE);
    }
    // BALI Note: check if ADSC is high here
    ADCSRA = _BV(ADEN) // turn ADC power on
  d2:	c0 93 7a 00 	sts	0x007A, r28
    uint16_t res = ADCL;
    return res | ADCH << 8;
  }
  
  static uint8_t adc_8bit(uint8_t goto_sleep_for_noise_reduction = 0) {
    busy_wait_adc_finished();
  d6:	0e 94 40 00 	call	0x80	; 0x80 <_ZN3AdcILN4_adc3RefE3EN6_ports3PinILNS2_4PortE1ELh5EEELNS0_4ModeE255ENS0_9DoNothingEE22busy_wait_adc_finishedEv>
    ADMUX |= _BV(ADLAR); // 8bit → left adjusted
  da:	80 91 7c 00 	lds	r24, 0x007C
  de:	80 62       	ori	r24, 0x20	; 32
  e0:	80 93 7c 00 	sts	0x007C, r24
    
      ADCSRA |= _BV(ADSC);
      sleep_mode();
    } else {
      // start conversion:
      ADCSRA |= _BV(ADSC);
  e4:	80 91 7a 00 	lds	r24, 0x007A
  e8:	80 64       	ori	r24, 0x40	; 64
  ea:	80 93 7a 00 	sts	0x007A, r24
    // start conversion:
    _start_adc(goto_sleep_for_noise_reduction);

    // leave busy_wait outside else branch, because sleep for noise reduction
    // might wake up because of another irq.
    busy_wait_adc_finished();
  ee:	0e 94 40 00 	call	0x80	; 0x80 <_ZN3AdcILN4_adc3RefE3EN6_ports3PinILNS2_4PortE1ELh5EEELNS0_4ModeE255ENS0_9DoNothingEE22busy_wait_adc_finishedEv>
  static void busy_wait_adc_finished() {
    loop_until_bit_is_clear(ADCSRA, ADSC);
  }
  
  static uint8_t get_adc_8bit_result() {
    return ADCH; // left adjusted → the 8 most significant bits are in ADCH!
  f2:	80 91 79 00 	lds	r24, 0x0079
  f6:	08 17       	cp	r16, r24
  f8:	10 f4       	brcc	.+4      	; 0xfe <main+0x74>
  fa:	5e 9a       	sbi	0x0b, 6	; 11
  fc:	01 c0       	rjmp	.+2      	; 0x100 <main+0x76>
      else reg &= ~_BV(bit);
  fe:	5e 98       	cbi	0x0b, 6	; 11
	#else
		//round up by default
		__ticks_dc = (uint32_t)(ceil(fabs(__tmp)));
	#endif

	__builtin_avr_delay_cycles(__ticks_dc);
 100:	8f e5       	ldi	r24, 0x5F	; 95
 102:	9a ee       	ldi	r25, 0xEA	; 234
 104:	01 97       	sbiw	r24, 0x01	; 1
 106:	f1 f7       	brne	.-4      	; 0x104 <main+0x7a>
 108:	00 c0       	rjmp	.+0      	; 0x10a <main+0x80>
 10a:	00 00       	nop
 10c:	c2 cf       	rjmp	.-124    	; 0x92 <main+0x8>

0000010e <_exit>:
 10e:	f8 94       	cli

00000110 <__stop_program>:
 110:	ff cf       	rjmp	.-2      	; 0x110 <__stop_program>
