#include "timer_pwm.h"

#include <avr/io.h>

void timer_pwm_16bit_50pct(uint32_t cycles) {
  TCCR1B = 0x18;  // Turn off for now
  uint8_t cs_mode = 1;
  if (cycles >= 0x01000000) {
    // 1024 scaling
    cycles >>= 10;
    cs_mode = 5;
  } else if (cycles >= 0x400000) {
    // 256 scaling
    cycles >>= 8;
    cs_mode = 4;
  } else if (cycles >= 0x080000) {
    // 64 scaling
    cycles >>= 6;
    cs_mode = 3;
  } else if (cycles >= 0x10000) {
    // 8 scaling
    cycles >>= 3;
    cs_mode = 2;
  }

  OCR1A = (uint16_t)cycles;
  TCCR1A = 0x43;  // COM1A0|WGM11|WGM10.  Toggle OC1A on match.  Count to OC1A
  TCCR1B = 0x18 | cs_mode;  // WGM12|WGM13
}

void timer_pwm_off() {
  TCCR1A = 0x00;
  TCCR1B = 0x00;
}
