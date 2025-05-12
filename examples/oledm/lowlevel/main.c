#include <oledm/oledm.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <oledm/ssd1306_init.h>

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

// Makes a | with a little right triangle at the bottom.
// The idea is to make it as easy as possible to figure out
// how the bits were mapped.
const uint8_t hi[] PROGMEM = {
    0xFF,
    0x07,
    0x03,
    0x01,
};

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  while (1) {
    _delay_ms(1000);
    oledm_start_pixels(&display);
    for (uint8_t i = 0; i < sizeof(hi); ++i) {
      oledm_write_pixels(&display, pgm_read_byte_near(hi + i));
    }
    oledm_stop(&display);
  }
}
