#ifndef LIB_SERVO_SERVO_ATTINY_H
#define LIB_SERVO_SERVO_ATTINY_H

// Control a servo using the ATTiny 85
//
// - Uses Timer 1. 
// - Supports 1mhz and 8mhz clock speeds
// - Resolution is 1us.
//
// IMPORTANT: Connecting a servo (or any motor) to the same
// VDD as the ATTiny will cause problems.
//
// Servo -> VDD Noise -> ATTiny VDD Noise -> PWM Noise -> 
//   Erratic Servo response
//
// It is recommended that the VDD for the ATTiny be isolated in some way.
// One way it to use a 3.3V Linear regulator (or similar) to power the
// ATTiny and filter away a good deal of the noise.

#include <inttypes.h>

// how many cycles to wait between pulses 153 ~= 20 ms but
// live tuning may be needed for accuracy due to differences in clocks
// from chip to chip
#define SERVO_COUNTER_20MS 153
// How many coarse cycles to delay to the fine buffer
// This creates more on an interrupt gap when the
// fine tune value is small.
#define SERVO_FINE_BUFFER 4

// Minimum and maximum servo ranges - to protect the servo itself
#define MIN_SERVO_PERIOD_US 800
#define MAX_SERVO_PERIOD_US 2200

// Process delays.  Determined empirically via an oscilliscope
#if F_CPU == 8000000
#define SERVO_PROCESS_DELAY_US 11
#else
#define SERVO_PROCESS_DELAY_US 86
#endif
// Process scale delays 64 -> period_us += period_us / 64
#define SERVO_PROCESS_SCALE -67

// Set or update servo frequency.
//   pin: PORTB pin number 0-5
//   period_id: Period in us.  Range from 800 - 2200
void servo_set(uint8_t pin, uint16_t period_us);

// Stop servo and set pin to a steady output low
//   hard_reset: If true then stop immediately which may truncate
//     an active live signal, communicating a shorter-than-intended signal.
//     If false, then wait for the signal to go low, which may
//     hang forever if interrupts are disabled.
void servo_stop(uint8_t hard_reset);

// Get currently set servo period.  Returns zero if servo_set was
// never called
uint16_t servo_get_period_us(void);

#endif
