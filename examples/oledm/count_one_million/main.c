#include <avr/interrupt.h>
#include <lowpower/lowpower.h>
#include <pstr/pstr.h>
#include <oledm/oledm.h>
#include <oledm/console.h>
#include <oledm/font/terminus6x8.h>

// Print from 0 to 1000000 with as little delay as possible.

// Wiring
//
//       +--------+
// N/C  -|        |- VDD
// N/C  -| Tiny85 |- SCL
// N/C  -|        |- N/C
// VSS  -|        |- SDA
//       +--------+
//
// Atmega328P:
//  A4 <- SDA
//  A5 <- SCL

//#define OLEDM_INIT ssd1306_64x32_a_init
//#define OLEDM_INIT ssd1306_128x32_a_init
#define OLEDM_INIT oledm_basic_init

struct Console console;
struct OLEDM display;

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);

  oledm_clear(&display, 0x00);
  console_init(&console, &display, terminus6x8);

  uint32_t i = 0;
  for (; i < 1000000; ++i) {
    console_pprint(&console, u32_to_ps(i));
    console_printlen(&console, "\n", 1);
  }

  lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}
