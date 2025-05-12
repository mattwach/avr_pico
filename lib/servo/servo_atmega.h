#ifndef LIB_SERVO_MULTI_SERVO_ATMEGA_H
#define LIB_SERVO_MULTI_SERVO_ATMEGA_H

// Control up to MAX_SERVO_SLOTS servos (configurable). 
//
// - Uses 16-bit timer (datasheet section 15: 16-bit Timer/Counter1 with PWM). 
// - Supports 16mhz clock speed
// - Resolution is 1us.
// 
// Servos are controlled one at a time.  Thus if you have three servos, the 
// sequence would be:
//
// 1) Send pulse for servo 0
// 2) Send pulse for servo 1
// 3) Send pulse for servo 2
// 4) Wait for pulse period to end, then loop back to 1
//
// IMPORTANT: Connecting a servo (or any motor) to the same
// VDD as the ATMega will cause problems.
// I suggest using a linear regulator for isolation
//
// API usage example with 2 servos:
//
// //First, init all servos.  This does not send any pulses yet
// servo_init_slot(0, 1, 0, 0, 1000);  // PB0
// servo_init_slot(1, 1 << 1, 0, 0, 2000);  // PB1
// set_servo_count(2);  // Servos are now getting a signal
//
// for (uint16_t i=0; i <= 1000; i += 10) {
//   servo_set_period_us(0, 1000 + i);  // sweep from 1000->2000
//   servo_set_period_us(1, 2000 - i);  // sweep from 2000->1000
//   delay_ms(100);
// } 
//
// set_servo_count(0);  // Turns off signals

#include <inttypes.h>

// Minimum and maximum servo ranges - to protect the servo itself
#define MIN_SERVO_PERIOD_US 800
#define MAX_SERVO_PERIOD_US 2200

#define PULSE_PERIOD_US 10000  // 10ms per servo pulse (100hz)

#define MAX_SERVO_SLOTS 4

#define DELAY_CYCLES 128  // setup delay, tuned empirically, using an oscilliscope

// initialize a servo slot with a given port mask.  The mask can
// be used to tie as many pins as needed to a given cycle, although
// one pin is common.
// The slot is ignored until servo_set_count() is called.
void servo_init_slot(
  uint8_t slot,
  uint8_t port_b_mask,
  uint8_t port_c_mask,
  uint8_t port_d_mask,
  uint16_t period_us);

// Set a servo's period in us
// Nothing happens until servo_set_count() is called once.
void servo_set_period_us(uint8_t slot, uint16_t period);

// Returns a servo's current period
uint16_t servo_get_period_us(uint8_t slot);

// changes the number of active servos.  This must be called
// one time before any servos will move.  It can also
// be called to change the number of active servos, including
// turning off all servo updates (by sending a zero). 
void servo_set_count(uint8_t num_slots);

#endif
