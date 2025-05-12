// This is a single-shot voltage monitor for a 1S battery.
// 
// It was built with the intention of measuring a 1S battery
//
// Wiring
//
//         +--------+
// N/C    -|        |- VDD
// VOLTIN -| Tiny85 |- SCL for display
// N/C    -|        |- N/C 
// VSS    -|        |- SDA for display
//         +--------+
//
// VOLTIN is from a voltage divider.
//

#include <adc/adc.h>
#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus16x32_numbers.h>
#include <util/delay.h>

#define SDA_PIN PB0
#define SCL_PIN PB2
#define VOLTIN_PIN PB3
#define VOLTIN_DIDR 3  // maybe the same as PB3, not sure

// ADC Samples are 0-1023.  All added we have 0-32736
// 32736 << 16 = 2145386496 with is less than 32 bits
// We then need to divide this number by something to
// get the actual voltage.  The exact number depends
// on the resistor divider networks, tolerances and
// the settling value of the voltage regulator
//
// To start, we assume that we are using the 1.1V
// internal source and want to measure up to 4.5V.
//
// We can get 10k (as recommended) total impedance
// via a GND --- 2.2k -|- 4.7k --- 2.2k
//
// at 4.5V, we would have 1.09V at the junction.
// we can assume this as 1023 for the starting
// divider calculation
//
// After a shift of 22, this would result in
// 0xFFC00000
//
// To get a final reading of 450 decivolts,
// we divide by 9535051.  That value will need
// to be tuned to account for real-world values.
#define ADC_RAW_SHIFT 22
#define ADC_RAW_DIVIDE 9454718

struct OLEDM display;
struct Text text;

int main(void) {
  // Set everything as an input to start
  DDRB = 0x00;
  // Set internal pullups
  PORTB = (1 << PB1) | (1 << PB4) | (1 << SDA_PIN) | (1 << SCL_PIN);
  // Disable digital input reads on the ADC pin
  DIDR0 = (1 << VOLTIN_DIDR);

  // Now wait for the capacitor that is buffering the battery and the
  // one that is stabilizing the ADC reading to charge
  lowpower_powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);

  // Do a one-shot ADC reading
  const uint16_t raw_adc_value = adc_quiet_read16(
      VOLTIN_PIN, ADC_REF_INTERNAL11, ADC_PRESCALER_128);

  // Now that we have the reading, we can power everything up to display it

  // Set SDA, SCL as low outputs
  PORTB &= ~((1 << SDA_PIN) | (1 << SCL_PIN));
  DDRB |= (1 << SDA_PIN) | (1 << SCL_PIN);

  // Initialize the OLED and display the voltage reading.
  ssd1306_64x32_a_init(&display);
  text_init(&text, terminus16x32_numbers, &display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  /*
  // Raw debug
  text.column = 0;
  text_char(&text, '0' + (char)(raw_adc_value / 1000));
  text_char(&text, '0' + (char)(raw_adc_value / 100 % 10));
  text_char(&text, '0' + (char)(raw_adc_value / 10 % 10));
  text_char(&text, '0' + (char)(raw_adc_value % 10));
  _delay_ms(1000);
  */

  const uint16_t volts = (uint16_t)(
      ((uint32_t)raw_adc_value << ADC_RAW_SHIFT) / ADC_RAW_DIVIDE);

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
