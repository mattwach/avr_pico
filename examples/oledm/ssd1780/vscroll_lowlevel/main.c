#include <avr/interrupt.h>
#include <pstr/pstr.h>
#include <oledm/oledm.h>
#include <util/delay.h>

#define OLEDM_INIT oledm_basic_init

struct OLEDM display;
uint8_t col = 0;

void box(uint16_t color) {
  display.fg_color = color;
  oledm_set_memory_bounds(&display, col, 0, 95, 7);
  oledm_start_pixels(&display);
  for (int i=0; i < 8; ++i) {
    oledm_write_pixels(&display, 0xFF);
    col = (col + 1) % 96;
  }
  oledm_stop(&display);
}

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  for (uint8_t i=0;; ++i) {
    box(rgb16_by_index(i));
    oledm_vscroll(&display, 1);
    _delay_ms(100);
  }

  while(1);

}
