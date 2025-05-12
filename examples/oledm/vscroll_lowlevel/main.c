#include <avr/interrupt.h>
#include <lowpower/lowpower.h>
#include <pstr/pstr.h>
#include <oledm/oledm.h>
#include <oledm/ssd1306_init.h>
#include <oledm/text.h>
#include <oledm/font/terminus6x8.h>

// A very simple and low level scrolling demo that prints
//
// Hello 1
//   Hello 2
//     Hello 3
// ...
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

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  text_init(&text, terminus6x8, &display);
  uint8_t i=0;
  for (; i < 8; ++i) {
    text.column = i * 2;
    text.row = i;
    text_str(&text, "Hi ");
    text_pstr(&text, u8_to_ps(i));
  }

  lowpower_powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);

  sei();

  while (1) {
    lowpower_powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    oledm_vscroll(&display, 1);
  }

}
