#ifndef READ_SERVO_PWM_H
#define READ_SERVO_PWM_H
// This library ties TIMER1 and INT0 to reading servo PWM

#include <inttypes.h>

// Set signal_lost_ms to determine determine when to set the
// pwm readout to zero.  A typical value is 150 (3 missed cycles)
void read_servo_pwm_init(uint16_t signal_lost_ms);

// Typically returns a value between 1000-2000 (in us).
// Some receivers go outside this range a bit (say 800-2200)
// If no signal is detected, then 0 is returned.
//
// Note that this function briefly disables interrupts, thus should
// not be called in a tight loop.
uint16_t read_servo_pwm();

#endif

