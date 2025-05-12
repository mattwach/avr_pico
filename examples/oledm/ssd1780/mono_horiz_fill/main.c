#include <oledm/oledm.h>

#define OLEDM_INIT oledm_basic_init

struct OLEDM display;

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);

  oledm_clear(&display, 0x00);

  uint8_t visible_rows = display.visible_rows / 8;
  oledm_set_bounds(&display, 0, 0, display.visible_columns - 1, visible_rows - 1);
  oledm_start_pixels(&display);
  const uint16_t length = display.visible_columns * visible_rows;
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
