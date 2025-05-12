// Oscillator that uses the AVRs built in 16-bit timer PWM feature
//
// Hardwre:
//   Potentiometer (POT)
//   128x32 I2C OLED display (OLED)
//   ATMega328P
// 
// Pin hookup:
//
// AVR ADC4 -> OLED SDA
// AVR ADC5 -> OLED SCLK
// AVR ADC3 -> POT Center
// AVR PB1  -> Output signal

#include <adc/adc.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus16x32_numbers.h>
#include <pstr/pstr.h>
#include <time/timer_pwm.h>
#include <util/delay.h>

#define OLEDM_INIT ssd1306_128x32_a_init
#define ADC_DIVISOR 32
#define MAX_FREQ_HZ 8000000
#define ADC_CHANNEL 3

struct OLEDM g_display;
struct Text g_text;
uint8_t g_adc_value;

uint32_t calc_frequency() {
  uint32_t freq_hz = 0;
  uint32_t adder = 2;
  uint32_t limit = 10;
  for (uint8_t adc_value = g_adc_value; adc_value > 0; --adc_value) {
    freq_hz += adder;
    if (freq_hz >= limit) {
      freq_hz = limit;
      adder *= 10;
      limit *= 10;
    }
  }
  if (freq_hz > MAX_FREQ_HZ) {
    freq_hz = MAX_FREQ_HZ;
  }
  return freq_hz;
}

int main(void) {
  DDRB |= 1 << 1;  // Set OC1A as an output
  OLEDM_INIT(&g_display);
  oledm_start(&g_display);
  oledm_clear(&g_display, 0x00);

  text_init(&g_text, terminus16x32_numbers, &g_display);

  while (1) {
    _delay_ms(15);
    const uint8_t adc_value =
        (uint8_t)(adc_read16(
            ADC_CHANNEL, ADC_REF_VCC, 7) / ADC_DIVISOR) + 1;
    if (adc_value == g_adc_value) {
      continue;
    }
    g_adc_value = adc_value;
    const uint32_t freq_hz = calc_frequency();
    timer_pwm_16bit_50pct(timer_pwm_hz_to_cycles(freq_hz));

    g_text.row = 0;
    g_text.column = 0;
    text_pstr(&g_text, u32_to_ps(freq_hz));
    text_char(&g_text, ' ');
  }
}
