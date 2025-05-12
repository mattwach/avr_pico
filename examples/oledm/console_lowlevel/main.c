#include <avr/interrupt.h>
#include <lowpower/lowpower.h>
#include <pstr/pstr.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus8x16.h>

// A very simple and low level "console demo"
//
// Hello 1
// Hello 2
// Hello 3
//
// Always prints on the last line, then scrolls.
//
// Then simply uses the vscroll in a loop.  This is not intended
// to be an example of how to make scrolling as use of a high-
// level library such as oledm/console.h will probably be
// easier.

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

#define OLEDM_INIT oledm_basic_init
//#define OLEDM_INIT ssd1306_64x32_a_init
//#define OLEDM_INIT ssd1306_128x32_a_init

struct OLEDM display;
struct Text text;
uint8_t line[32];

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  text_init(&text, terminus8x16, &display);

		sei();
		uint16_t i=0;

		for (;; ++i) {
				lowpower_powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
				oledm_vscroll(&display, 2);

				text.column = 0;
				text.row = 6;
				cs_to_ps_cpy(line, "Hello ");
				pstrcat(line, u16_to_ps(i));
				cs_to_ps_cat(line, " ");
				text_pstr(&text, line);
		}
}
