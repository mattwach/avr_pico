#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/font/terminus8x16.h>
#include <util/delay.h>

#define OLEDM_INIT oledm_basic_init

struct OLEDM display;
struct Text text;

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  uint8_t col = 0;
  uint8_t dir = 1;
  uint8_t row = 0;
  while (1) {
    text_init(&text, terminus8x16, &display);
    text.row = row;
    text.column = col;
    display.fg_color = rgb16_by_index(text.row + 1);
    text_str(&text, " Hello World! ");

    col += dir;
    if (col == 0 || col == 95) {
      dir = -dir;
      if (col == 95) {
        row = (row + 1) & 0x07;
      }
    }
    _delay_ms(16);
  }
}
