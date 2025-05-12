#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#define BUZZER_PIN 5  // OC1B
#define LOWER_PIN 0   // PA0
#define HIGHER_PIN 1   // PA1

#define MIN_FREQ_HZ 500
#define START_FREQ_HZ 2000
#define MAX_FREQ_HZ 5000
#define FREQ_STEP_HZ 100

static uint8_t buzzer_freq_hz = 0;

static void set_buzzer_freq_hz(uint16_t freq_hz) {
  if (freq_hz == buzzer_freq_hz) {
    return;
  }
  buzzer_freq_hz = freq_hz;
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  if (buzzer_freq_hz == 0) {
    return;
  }
  // support hz from 50 to 10000
  // for a 1Mhz cpu, cycles will vary from 10000 to 50
  // for a 8mhz cpu, cycles will vary from 80000 to 400
  uint32_t cycles = F_CPU / 2 / freq_hz;
  uint8_t cs_mode = 1;
  if (cycles >= 0x10000) {
    // 8 scaling
    cycles >>= 3;
    cs_mode = 2;
  }

  OCR1A = (uint16_t)cycles;
  TCCR1A = (1 << COM1B0);  // Toggle on compare match
  TCCR1B = (1 << WGM12) | cs_mode;  // Non PWM mode, OCR1A is TOP
}

int main(void) {
  DDRA = 1 << BUZZER_PIN;
  set_buzzer_freq_hz(20000);
  while (1);
}
