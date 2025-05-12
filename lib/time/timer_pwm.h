#ifndef TIMER_PWM_H
#define TIMER_PWM_H
// Simplifies setting up PWM for different purposes

#include <inttypes.h>

// convert a peroid in us to a cycle count
static inline uint32_t timer_pwm_us_to_cycles(uint32_t us) {
  // 2000000 instead of 1000000 because a period has 2 inversions
  return us * (F_CPU / 2000000);
}

// convert hertz to a cycle count
static inline uint32_t timer_pwm_hz_to_cycles(uint32_t hz) {
  return F_CPU / 2 / hz;
}

// Sets up a 50% duty cycle timer that toggles every given cycles
void timer_pwm_16bit_50pct(uint32_t cycles);

// tunrs off timer
void timer_pwm_off();

#endif
