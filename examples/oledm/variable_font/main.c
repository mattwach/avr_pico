#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <util/delay.h>
#include "hello_world_font.h"

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

  uint8_t col = 0;
  uint8_t dir = 1;
  uint8_t row = 0;
  while (1) {
    text_init(&text, hello_world_font, &display);
    text.row = row;
    text.column = col;
    text_str(&text, " Hello World! ");

    col += dir;
    if (col == 0 || col == 127) {
      dir = -dir;
      if (col == 127) {
        row = (row + 1) & 0x07;
      }
    }
    _delay_ms(16);
  }
}
