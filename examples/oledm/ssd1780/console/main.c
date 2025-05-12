#include <oledm/oledm.h>
#include <oledm/console.h>
#include <oledm/font/terminus8x16.h>
#include <util/delay.h>
#include <pstr/pstr.h>

#define OLEDM_INIT oledm_basic_init

struct OLEDM display;
struct Console console;

int main(void) {
  OLEDM_INIT(&display);

  oledm_start(&display);
  oledm_clear(&display, 0x00);

  console_init(&console, &display, terminus8x16);
  uint32_t i = 0;
  for (;; ++i) {
    display.fg_color = rgb16_by_index(i & 0xFF);
    console_pprint(&console, u32_to_ps(i));
    if (i & 1) {
      console_printlen(&console, "\n", 1);
    } else {
      console_printlen(&console, " ", 1);
    }
    _delay_ms(50);
  }
}
