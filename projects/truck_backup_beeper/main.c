#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#define PIN 3 
// Going for 1900 Hz, will need to tune on the oscilliscope
#define HALF_WAVE_PEROID_US 259
// how many waves to do before turning off (and back on again)
#define PERIOD_COUNT (400000 / HALF_WAVE_PEROID_US)

int main(void) {
  DDRB = 1 << PIN;
  uint8_t is_on = 1;
  while (1) {
    for (uint32_t i = 0; i < PERIOD_COUNT; ++i) {
      if (is_on) {
        PORTB |= 1 << PIN;
      }
      _delay_us(HALF_WAVE_PEROID_US);
      PORTB &= ~(1 << PIN);
      _delay_us(HALF_WAVE_PEROID_US);
    }
    is_on = 1 - is_on;
  }
}
