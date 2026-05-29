#include "read_servo_pwm.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static uint32_t signal_lost_ms;
volatile static uint32_t rollover_ms;
volatile static uint16_t falling_edge_ticks;

#define MIN_PWM_US 500
#define MAX_PWM_US 2500

#define INT0_DDR DDRD
#define INT0_PORT PORTD
#define INT0_INPUT PIND
#define INT0_PIN 2

void read_servo_pwm_init(uint16_t signal_lost_ms_) {
  signal_lost_ms = signal_lost_ms_;
  rollover_ms = 0;
  falling_edge_ticks = 0;

  // Disable global interrupts during setup
  cli();

  // Reset Timer1
  TCCR1A = 0;
  TCCR1B = 0;

  // Timer setup
  TCNT1 = 0;        // initial timer value
  OCR1A = 19999;    // compare value has a range of 20000 0.5 us ticks (10ms per interrupt)
  TCCR1B |= 
    (1 << WGM12) |  // clear on compare
    (1 << CS11);    // /8 prescaler (1 tick every 0.5 us)  

  // Enable timer1 compare match A Interrupt
  TIMSK1 |= (1 << OCIE1A);

  // setup the signal monitor
  EICRA |= (1 << ISC00);  // Any logic change on INT0 generates an interrupt.
  EIMSK |= (1 << INT0);  // Enable the interrupt

  // Enable global interrupts
  sei();
}

uint16_t read_servo_pwm() {
  uint8_t sreg = SREG;
  cli();
  const uint32_t over_ms = rollover_ms;
  const uint16_t ticks = falling_edge_ticks;
  SREG = sreg;
  const uint16_t width_us = ticks >> 1;
  if ((over_ms >= signal_lost_ms) || (width_us < MIN_PWM_US) || (width_us > MAX_PWM_US)) {
    return 0;
  }
  return width_us;
}

ISR(TIMER1_COMPA_vect) {
  rollover_ms += 10;
}

ISR(INT0_vect) {
  if (INT0_INPUT & (1 << INT0_PIN)) {
    TCNT1 = 0;  // reset the counter
    rollover_ms = 0;
  } else {
    falling_edge_ticks = TCNT1;
  }
}

