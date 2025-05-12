#include "servo_atmega.h"
#include "error_codes.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

struct ServoSlot {
  uint8_t port_b_mask;
  uint8_t port_c_mask;
  uint8_t port_d_mask;
  uint16_t cycles;
  uint16_t period_us; // alias to cycles, used for tracking g_wait_sum
};

volatile struct ServoSlot g_servo_slot[MAX_SERVO_SLOTS];
volatile uint8_t g_num_servos = 0;
volatile uint8_t g_current_servo = 0;
volatile uint16_t g_wait_sum = 0; // used to subtract servo active time from cycle time

static inline uint16_t calc_cycles(uint16_t period_us) {
  return period_us * 16 - DELAY_CYCLES;  // Assuming 16Mhz at 1 cycles/count.  This will likely need tweaking.
}

void servo_init_slot(
    uint8_t slot,
    uint8_t port_b_mask,
    uint8_t port_c_mask,
    uint8_t port_d_mask,
    uint16_t period_us) {
  if (slot >= MAX_SERVO_SLOTS) {
    return;
  }
  volatile struct ServoSlot* ss = g_servo_slot + slot;
  ss->port_b_mask = port_b_mask;
  DDRB |= port_b_mask;
  ss->port_c_mask = port_c_mask;
  DDRC |= port_c_mask;
  ss->port_d_mask = port_d_mask;
  DDRD |= port_d_mask;
  ss->period_us = 0; // don't mess up g_wait_sum
  servo_set_period_us(slot, period_us);
}

void servo_set_period_us(uint8_t slot, uint16_t period) {
  if (slot >= MAX_SERVO_SLOTS) {
    return;
  }
  if (period < MIN_SERVO_PERIOD_US) {
    period = MIN_SERVO_PERIOD_US;
  } else if (period > MAX_SERVO_PERIOD_US) {
    period = MAX_SERVO_PERIOD_US;
  }

  volatile struct ServoSlot* ss = g_servo_slot + slot;
  if (period == ss->period_us) {
    // no change
    return;
  }

  // Note that every us the interrupts are disabled will potentially
  // extend timing windows in a way that makes the servos actually move.
  // For this reason, we keep the window as short as possible
  const uint16_t new_wait_sum = g_wait_sum - ss->period_us + period;
  const uint16_t new_cycles = calc_cycles(period);

  cli(); // Avoid races with COMPA interrupt
  g_wait_sum = new_wait_sum;
  ss->cycles = new_cycles;
  sei();
  ss->period_us = period;
}

uint16_t servo_get_period_us(uint8_t slot) {
  return g_servo_slot[slot].period_us;
}

static inline void lower_signal(uint8_t slot) {
  const volatile struct ServoSlot* ss = g_servo_slot + slot;
  PORTB &= ~(ss->port_b_mask);
  PORTC &= ~(ss->port_c_mask);
  PORTD &= ~(ss->port_d_mask);
}

static inline void raise_current_servo_signal_and_set_timer() {
  const volatile struct ServoSlot* ss = g_servo_slot + g_current_servo;
  OCR1A = ss->cycles;
  PORTB |= ss->port_b_mask;
  PORTC |= ss->port_c_mask;
  PORTD |= ss->port_d_mask;
  TCCR1B = 0x01;
}

static void start_servos() {
  // OC1A/OX1B disconnected.  Normal timer operations
  TCCR1A = 0x00;

  // Turn off the timer if it's running
  TCCR1B = 0x00;

  // Start with zero
  TCNT1 = 0x0000;

  // enable the counter match interrupt
  TIMSK1 |= (1 << OCIE1A);

  // set the current servo and raise the pins
  g_current_servo = 0;
  raise_current_servo_signal_and_set_timer();

  // enable general interrupts
  sei();
}

static void stop_servos() {
  // Turn off the timer
  TCCR1B = 0x00;

  // clear the counter match interrupt
  TIMSK1 &= ~(1 << OCIE1A);

  // drop all pins
  for (uint8_t i=0; i < g_num_servos; ++i) {
    lower_signal(i);
  }
}

void servo_set_count(uint8_t num_servos) {
  if (num_servos == 0 && g_num_servos > 0) {
    // dont wait more than 6 seconds
    for (uint16_t i=0; i < 60000 && g_current_servo < g_num_servos; ++i) {
      // We are mid-cycle.  Wait for the cycle to end
      _delay_ms(1);
    }
    stop_servos();
    g_num_servos = num_servos;
  } else if (g_num_servos == 0 && num_servos > 0) {
    g_num_servos = num_servos;
    start_servos();
  } else {
    cli();
    g_num_servos = num_servos;
    sei();
  }
}

ISR(TIMER1_COMPA_vect) {
  // Turn off the timer and reset counter for next iteration
  TCCR1B = 0x00;
  TCNT1 = 0x0000;

  if (g_current_servo >= g_num_servos) {
    // starting a new cycle
    g_current_servo = 0;
  } else {
    // onto the next servo, or end-of-cycle (see below)
    lower_signal(g_current_servo);
    ++g_current_servo;
    if (g_current_servo >= g_num_servos && g_wait_sum >= PULSE_PERIOD_US) {
      // No time left, just get going on the next iteration
      g_current_servo = 0;
    }
  }

  if (g_current_servo < g_num_servos) {
    raise_current_servo_signal_and_set_timer();
  } else {
    // wait for the next period
    OCR1A = (PULSE_PERIOD_US - g_wait_sum) * 2;  // assume 16mhz and 8 clocks / cycle
    TCCR1B = 0x02; // start timing at 8 cycle/count
  }
}
