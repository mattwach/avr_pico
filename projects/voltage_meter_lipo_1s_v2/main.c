// This is a single-shot voltage monitor for a 1S battery.
// 
// It was built with the intention of measuring a 1S battery
//
// Wiring
//
//         +--------+
// N/C    -|        |- VDD
// N/C    -| Tiny85 |- SCL for display
// N/C    -|        |- N/C 
// VSS    -|        |- SDA for display
//         +--------+
//

#include <adc/adc.h>
#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <util/delay.h>
#include "number_font.h"

#define SDA_PIN PB0
#define SCL_PIN PB2

// The raw value represents 1.1 volts.  To figure out the real voltage
// we need to take 110 (decivolts) * 1023 / raw_value
//
// 5V example:
// raw_value = 225, 112530 / 225 = 500
// 1.1V Example
// raw_value = 1023, 112530 / 1023 = 110
// That said, the numerator is set as a tunable constant
#define VOLTAGE_REF_NUMERATOR 112530

struct OLEDM display;
struct Text text;

#define RAW_ADC_VALUE_SAMPLES 8
uint16_t raw_adc_values[RAW_ADC_VALUE_SAMPLES];
uint32_t raw_adc_sum;

// One time initialization
void init() {
  // Set everything as an input to start
  DDRB = 0x00;
  // Set internal pullups
  PORTB = (1 << PB1) | (1 << PB3) | (1 << PB4) | (1 << SDA_PIN) | (1 << SCL_PIN);

  // Set SDA, SCL as low outputs
  PORTB &= ~((1 << SDA_PIN) | (1 << SCL_PIN));
  DDRB |= (1 << SDA_PIN) | (1 << SCL_PIN);

  // wait for OLED to be ready
  _delay_ms(50);

  // Initialize the OLED and display the voltage reading.
  oledm_basic_init(&display);
  text_init(&text, number_font, &display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
}

// Reads an adc sample and updates raw_adc_sum
void sample_adc(int i) {
  i = i % RAW_ADC_VALUE_SAMPLES;
  raw_adc_sum -= raw_adc_values[i];
  raw_adc_values[i] = adc_quiet_read16_internal_ref(ADC_PRESCALER_128);
  raw_adc_sum += raw_adc_values[i];
}

// Prints raw_adc_sum / RAW_ADC_VALUE_SAMPLES to OLED
void print_value_to_oled() {
  // Raw debug
  uint16_t raw_adc_value = (uint16_t)(raw_adc_sum / RAW_ADC_VALUE_SAMPLES);
  text.column = 0;
  text_char(&text, '0' + (char)(raw_adc_value / 1000));
  text_char(&text, '0' + (char)(raw_adc_value / 100 % 10));
  text_char(&text, '0' + (char)(raw_adc_value / 10 % 10));
  text_char(&text, '0' + (char)(raw_adc_value % 10));

  /*
  const uint16_t volts = (uint16_t)(VOLTAGE_REF_NUMERATOR / raw_adc_value);

  text.column = 0;
  text_char(&text, '0' + (char)(volts / 100));
  text_char(&text, '.');
  text_char(&text, '0' + (char)((volts / 10) % 10));
  text_char(&text, '0' + (char)(volts % 10));
  */
}

// Program entry point
int main(void) {
  init();
  for (uint32_t i=0;;++i) {
    sample_adc(i);
    if (i < RAW_ADC_VALUE_SAMPLES) {
      _delay_ms(20);
      continue;
    }
    print_value_to_oled();
    _delay_ms(200);
  }
  return 0;
}
