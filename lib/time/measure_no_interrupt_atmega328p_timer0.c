#include <time/measure_no_interrupt.h>

#include <avr/io.h>

static uint16_t g_shift;

void measure_ni_init(uint8_t divider) {
  if (divider <= 3) {
    g_shift = (divider - 1) * 3;
  } else  {
    g_shift = divider * 2;
  }

  // OC0A disconnected, OC0B disconnected, normal timer
  TCCR0A = 0x00;

  // No interrupts
  TIMSK0 = 0x00;

  // Set timer counting
  TCCR0B = divider & 0x07;
}

void measure_ni_reset(void) {
  // reset timer and clear overflow flags
  TCNT0 = 0x00;
  TIFR0 = 0x01;  // Reset TOV0
}

#define MEASURE_COMMON(DIVIDER) \
  const bool_t at_max = TIFR0 & 0x01;\
  if (is_max) *is_max = at_max;\
  const uint32_t counter = at_max ? 0xFF : TCNT0;\
  return (uint16_t)((counter << g_shift) / (F_CPU / DIVIDER));

uint16_t measure_ni_time_us(bool_t* is_max) {
  MEASURE_COMMON(1000000)
}

uint16_t measure_ni_time_ms(bool_t* is_max) {
  MEASURE_COMMON(1000)
}
