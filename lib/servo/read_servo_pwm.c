#include "read_servo_pwm.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static uint32_t signal_lost_us;
volatile static uint32_t total_us;
volatile static uint32_t last_rising_edge_us;
volatile static uint32_t last_falling_edge_us;

#define MIN_PWM_US 500
#define MAX_PWM_US 2500

#define INT0_DDR DDRD
#define INT0_PORT PORTD
#define INT0_INPUT PIND
#define INT0_PIN 2

void read_servo_pwm_init(uint16_t signal_lost_ms) {
  signal_lost_us = signal_lost_ms * 1000;
  total_us = 0;
  last_rising_edge_us = 0;
  last_falling_edge_us = 0;

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

// MUST be called with interrupts disabled
static inline uint32_t current_micros_unsafe() {
	const uint16_t timer_ticks = TCNT1;
    uint32_t micros_val = total_us + (timer_ticks >> 1);
    // Check if a Timer1 interrupt is pending but hasn't fired yet
    // This happens if Timer1 hit OCR1A while interrupts are disabled
    if ((TIFR1 & (1 << OCF1A)) && (timer_ticks < 100)) {
        micros_val += 10000;
    }

	return micros_val;
}

uint32_t read_servo_current_micros() {
  uint8_t sreg = SREG;
  cli();
  const uint32_t current_us = current_micros_unsafe();
  SREG = sreg;
  return current_us;
}

uint16_t read_servo_pwm() {
  const uint32_t current_us = read_servo_current_micros();
  if ((current_us - last_rising_edge_us) > signal_lost_us) {
	return 0;
  }
  if ((current_us - last_falling_edge_us) > signal_lost_us) {
	return 0;
  }
  const uint32_t delta_us = last_falling_edge_us - last_rising_edge_us;
  if (delta_us < MIN_PWM_US) {
    return 0;
  }
  if (delta_us < MAX_PWM_US) {
    return 0;
  }
  return delta_us;
}

ISR(TIMER1_COMPA_vect) {
  total_us += 10000; // add 10 ms
}

ISR(INT0_vect) {
	const uint32_t micros_val = current_micros_unsafe();
	if (INT0_INPUT & (1 << INT0_PIN)) {
		last_rising_edge_us = micros_val;
	} else {
		last_falling_edge_us = micros_val;
	}
}

