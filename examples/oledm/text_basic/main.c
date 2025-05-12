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

  oledm_set_bounds(&display, 0, 0, display.visible_columns - 1, 0);
  oledm_start_pixels(&display);
  int x = 0;
  for (; x < display.visible_columns; ++x) {
    uint8_t b = 0x00;
    if ((x == 0) || (x >= 127)) {
      b = 0xFF;
    } else if (!(x & 0x07)) {
      if (!(x & 0x0F)) {
        b = 0x0F;
      } else {
        b = 0x03;
      }
    }
    oledm_write_pixels(&display, b);
  }
  oledm_stop(&display);

  text_init(&text, terminus6x8, &display);
  text.row = 2;
  text_str(&text, "Hello Washington!");
}
