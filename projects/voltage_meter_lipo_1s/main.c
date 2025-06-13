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

#include "large_font.h"
#include <adc/adc.h>
#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <util/delay.h>

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

int main(void) {
  // Set everything as an input to start
  DDRB = 0x00;
  // Set internal pullups
  PORTB = (1 << PB1) | (1 << PB3) | (1 << PB4) | (1 << SDA_PIN) | (1 << SCL_PIN);

  // Now wait for the capacitor that is buffering the battery and the
  // one that is stabilizing the ADC reading to charge
  lowpower_powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);

  // Do a one-shot ADC reading
  const uint16_t raw_adc_value = adc_quiet_read16_internal_ref(
      ADC_PRESCALER_128);

  // Now that we have the reading, we can power everything up to display it

  // Set SDA, SCL as low outputs
  PORTB &= ~((1 << SDA_PIN) | (1 << SCL_PIN));
  DDRB |= (1 << SDA_PIN) | (1 << SCL_PIN);

  // Initialize the OLED and display the voltage reading.
  ssd1306_64x32_a_init(&display);
  text_init(&text, large_font, &display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  const uint16_t volts = (uint16_t)(VOLTAGE_REF_NUMERATOR / raw_adc_value);

  text.column = 0;
  text_char(&text, '0' + (char)(volts / 100));
  text_char(&text, '.');
  text_char(&text, '0' + (char)((volts / 10) % 10));
  text_char(&text, '0' + (char)(volts % 10));

  // just sleep
  while (1) {
    lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}
