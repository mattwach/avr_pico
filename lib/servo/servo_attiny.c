#include "servo_attiny.h"

#include <avr/interrupt.h>
#include <avr/io.h>

// Interrupt state machine
#define STATE_INIT        0  // Interrupts should be off
#define STATE_WAIT        1  // Timer is doing the 20ms wait
#define STATE_ARM_COARSE  2  // Signal is armed, with initial timing
#define STATE_ARM_FINE    3  // Signal is armed, with initial timing

uint8_t pin_mask_;
volatile uint16_t period_ = 0;
volatile uint16_t specified_period_ = 0;
uint16_t next_period_;
volatile uint8_t state_ = STATE_INIT;


void servo_set(uint8_t pin, uint16_t period) {
  if (period < MIN_SERVO_PERIOD_US) {
    period = MIN_SERVO_PERIOD_US;
  }
  if (period > MAX_SERVO_PERIOD_US) {
    period = MAX_SERVO_PERIOD_US;
  }

  cli();

  specified_period_ = period;

  // Account for the delays in the code
  period -= SERVO_PROCESS_DELAY_US;

  if (SERVO_PROCESS_SCALE > 0) {
    period += period / SERVO_PROCESS_SCALE;
  } else if (SERVO_PROCESS_SCALE < 0) {
    period -= period / (-SERVO_PROCESS_SCALE);
  }

  const uint8_t new_pin_mask = 1 << pin;
  if (new_pin_mask != pin_mask_) {
    // clear the old pin and the new one too
    PORTB &= ~(pin_mask_ | new_pin_mask);
    DDRB |= new_pin_mask;
  }

  pin_mask_ = new_pin_mask;
  // If in STATE_ARM, we can't change period or
  // the in-flight calculations will be altered,
  // resulting in a corrupted time period.
  if ((state_ != STATE_ARM_COARSE) && (state_ != STATE_ARM_FINE)) {
    period_ = period;
  }
  next_period_ = period;

  if (state_ != STATE_INIT) {
    sei();
    return;
  }

  // Stop the timer
  TCCR1 = 0x00;

  // Use the synchronous clock
  PLLCSR = 0x00;

  // No PWM
  // Don't change COM1B
  GTCCR = 0x00;

  // Start with zero
  TCNT1 = 0x00;

  // 156 will put us very close to 20 ms
  // (50Hz)
  OCR1A = SERVO_COUNTER_20MS;

  // Interrupt on Counter 1 match
  TIMSK |= (1 << OCIE1A);

  state_ = STATE_WAIT;

  // Don't clear counters on match.  We'll do it
  // No PWM - 256 steps is not enough
  // Each tick will be approx 128 us
#if F_CPU == 8000000
  TCCR1 = 0x0B;  // div by 1024
#else
  TCCR1 = 0x08;  // div by 128
#endif

  // Interrupts on
  sei();
}

uint16_t servo_get_period_us(void) {
  return specified_period_;
}

void servo_stop(uint8_t hard_reset) {
  // Wait for any pending ARMS to complete
  if (hard_reset) {
    cli();
  } else {
    while (1) {
      cli();
      if ((state_ != STATE_ARM_COARSE) && (state_ != STATE_ARM_FINE)) {
        break;
      }
      sei();
    }
  }
  // Clear the pin
  PORTB &= ~(pin_mask_);
  pin_mask_ = 0x00;
  period_ = 0;
  next_period_ = 0;
  specified_period_ = 0;
  state_ = STATE_INIT;
  // Stop the timer
  TCCR1 = 0x00;
  // Interrupt on Counter 1 clear
  TIMSK &= ~(1 << OCIE1A);
  // Interrupts on
  sei();
}

ISR(TIMER1_COMPA_vect) {
  switch (state_) {
    case STATE_WAIT:
      // Stop the timer
      TCCR1 = 0x00;
      // Reset the counter
      TCNT1 = 0x00;
      // uing an / 16 counter (at 1mhz)
      // - 32 to give the fine counter a bit of buffer
      OCR1A = (period_ >> 4) - SERVO_FINE_BUFFER;
      // Enable timer with said period
#if F_CPU == 8000000
      TCCR1 = 0x08;  // div by 128
#else
      TCCR1 = 0x05;  // div by 16
#endif
      PORTB |= pin_mask_;
      state_ = STATE_ARM_COARSE;
      break;
    case STATE_ARM_COARSE:
      // Stop the timer
      TCCR1 = 0x00;
      // Reset the counter
      TCNT1 = 0x00;
      // pick up the rest of the bits, plus some buffer
      // to give the intterupt some space to trigger.
      OCR1A = (period_ & 0x0F) + (SERVO_FINE_BUFFER << 4);
      // Enable timer with said period
#if F_CPU == 8000000
      TCCR1 = 0x04;  // div by 8
#else
      TCCR1 = 0x01;  // no divide
#endif
      state_ = STATE_ARM_FINE;
      break;
    case STATE_ARM_FINE:
      PORTB &= ~pin_mask_;
      // Stop the timer
      TCCR1 = 0x00;
      // Reset the counter
      TCNT1 = 0x00;
      // 20ms
      // Try to account for the period in the next calculation
      OCR1A = SERVO_COUNTER_20MS - (specified_period_ >> 7);
      // Enable timer with said period
#if F_CPU == 8000000
      TCCR1 = 0x0B;  // div by 1024
#else
      TCCR1 = 0x08;  // div by 128
#endif
      period_ = next_period_;
      state_ = STATE_WAIT;
      break;
  }
}
