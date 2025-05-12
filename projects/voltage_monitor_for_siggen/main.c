// This is a simple voltage monitor for a 2S battery.
// 
// It was built with the intention of monitoring the 2S battery
// conencted to a signal generator
//
// Wiring
//
//         +--------+
// N/C    -|        |- VDD
// VOLTIN -| Tiny85 |- SCL for display
// ALERT  -|        |- N/C 
// VSS    -|        |- SDA for display
//         +--------+
//
// VOLTIN is from a voltage divider.  Alert is to a diode/resistor.
//
// For voltage we want to take several samples over ~ 250ms and average them
// in a rolling-average style
//
// Accuracy is recored as integers in 10mv increments.  e.g.  654 => 6.54 volts

#include <adc/adc.h>
#include <avr/io.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus16x32_numbers.h>
#include <util/delay.h>

#define VOLTIN_PIN PB3
#define ALERT_PIN PB4
#define SAMPLE_COUNT 64
#define MAX_VOLTS 999  // in 10mv increments

#define CRITICAL_VOLTAGE 720
#define CRITICAL_DELAY 4
#define WARNING_VOLTAGE 758
#define WARNING_DELAY 120

// ADC Samples are 0-1023.  All added we have 0-32736
// 32736 << 16 = 2145386496 with is less than 32 bits
// We then need to divide this number by something to
// get the actual voltage.  The exact number depends
// on the resistor divider networks, tolerances and
// the settling value of the voltage regulator
//
// To start, we assume that we have a 5V regulator
// 8.4V on the battery and a 50% divider network
// That would read as 4.2V or 860 on the ADC
// 32 samples would be 860 * 32 = 25720
// After the shift, we have 25720 << 16 = 1803550720
// We want that to equate to 840 so the divider
// would be 1803550720 / 840 = 2147084
#define SAMPLE_SHIFT 15
#define SAMPLE_DIVIDER 2431196

uint16_t samples[SAMPLE_COUNT];
uint32_t cycle;
uint32_t next_blink;
uint16_t last_volts;
struct OLEDM display;
struct Text text;

uint16_t get_voltage(void) {
  samples[cycle % SAMPLE_COUNT] = adc_read16(
      VOLTIN_PIN, ADC_REF_VCC, ADC_PRESCALER_128);

  // This could be a rolling average, but we have plenty of time
  uint32_t volts = 0;
  int8_t i = 0;
  for (; i < SAMPLE_COUNT; ++i) {
    volts += samples[i];
  }

  volts <<= SAMPLE_SHIFT;
  volts /= SAMPLE_DIVIDER;
  if (volts > MAX_VOLTS) {
    volts = MAX_VOLTS;
  }

  return volts;
}

void alert(uint8_t on) {
  if (on) {
    PORTB |= 1 << ALERT_PIN;
  } else {
    PORTB &= ~(1 << ALERT_PIN);
  }
}

void maybe_blink(uint16_t volts) {
  if (next_blink >= cycle) {
    alert(next_blink == cycle);
    return;
  }

  if (volts > WARNING_VOLTAGE) {
    alert(0);
    return;
  }

  if (volts < CRITICAL_VOLTAGE) {
    volts = CRITICAL_VOLTAGE;
  }

  // endpoint checks
  //
  // if volts == CRITICAL_VOLTAGE
  // then delay = CRITICAL_DELAY + 0
  //
  // if volts == WARNING_VOLTAGE
  // then delay = CRITICAL_DELAY + (WARNING_DELAY - CRITICAL_DLAY) * 1
  //            = WARNING_DELAY
  uint16_t delay = CRITICAL_DELAY + 
    (WARNING_DELAY - CRITICAL_DELAY) *
    (volts - CRITICAL_VOLTAGE) /
    (WARNING_VOLTAGE - CRITICAL_VOLTAGE);

  next_blink = cycle + delay;
}

void iterate(void) {
  const uint16_t volts = get_voltage();
  maybe_blink(volts);

  if (volts == last_volts) {
    return;
  }

  last_volts = volts;
  text.column = 0;
  text_char(&text, '0' + (char)(volts / 100));
  text_char(&text, '.');
  text_char(&text, '0' + (char)((volts / 10) % 10));
  text_char(&text, '0' + (char)(volts % 10));
}

int main(void) {
  cycle = 0;
  // Make every pin an output pin except for the ADC
  PORTB = 0x00;
  DDRB = 0xFF & ~(1 << VOLTIN_PIN);

  ssd1306_64x32_a_init(&display);
  text_init(&text, terminus16x32_numbers, &display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  while (1) {
    iterate();
    _delay_ms(15);
    ++cycle;
  }
}
