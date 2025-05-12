#include <avr/interrupt.h>
#include <lowpower/lowpower.h>
#include <pstr/pstr.h>
#include <oledm/oledm.h>
#include <oledm/console.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus6x8.h>
#include <adc/adc.h>

// A very simple and low level "console demo"
//
// Prints the current values of ADC "AO" as a raw value
// and estimated voltage.
//
// Updates 4 times per second

// Wiring
//
//       +--------+
// N/C  -|        |- VDD
// N/C  -| Tiny85 |- SCL
// N/C  -|        |- ADC
// VSS  -|        |- SDA
//       +--------+
//
// Atmega328P:
//  A4 <- SDA
//  A5 <- SCL
//  ADC1 <- ADC

#define OLEDM_INIT oledm_basic_init
//#define OLED64x32
//#define OLEDM_INIT ssd1306_64x32_a_init
//#define OLEDM_INIT ssd1306_128x32_a_init
#define ADC_CHANNEL 1

struct Console console;
struct OLEDM display;
uint8_t line[32];

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);

  oledm_clear(&display, 0x00);
  console_init(&console, &display, terminus6x8);

  sei();
  uint16_t i = 0;

  for (;; ++i) {
    lowpower_powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);

    uint16_t adc_val = adc_read16(ADC_CHANNEL, ADC_REF_VCC, ADC_PRESCALER_128);
    uint8_t decivolts = (uint8_t)((adc_val * 50) / 1024);

    console_pprint(&console, u16_to_ps(i));
    console_print(&console, ": ");
    console_pprint(&console, u16_to_ps(adc_val));
#ifdef OLED64x32
    console_print(&console, "\n");
#else
    console_print(&console, " (");
    console_pprint(&console, u8_to_ps(decivolts / 10));
    console_print(&console, ".");
    console_pprint(&console, u8_to_ps(decivolts % 10));
    console_print(&console, "V)\n");
#endif
  }
}
