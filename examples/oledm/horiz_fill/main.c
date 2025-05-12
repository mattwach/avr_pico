#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus6x8.h>

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

  oledm_set_bounds(&display, 0, 0, display.visible_columns - 1, display.visible_rows - 1);
  oledm_start_pixels(&display);
  const uint16_t length = display.visible_columns * display.visible_rows;
  uint16_t i = 0;
  for (; i <length; ++i) {
    uint8_t x = i % display.visible_columns;
    uint8_t b = 0x00;
    if (x == (display.visible_columns - 1)) {
      b = 0xFF;
    } else if (!(x & 0x07)) {
      if (!(x & 0x0F)) {
        if (!(x & 0x3f)) {
          b = 0xFF;
        } else {
          b = 0x0F;
        }
      } else {
        b = 0x03;
      }
    }
    oledm_write_pixels(&display, b);
  }
  oledm_stop(&display);

  while (1) { }
}
