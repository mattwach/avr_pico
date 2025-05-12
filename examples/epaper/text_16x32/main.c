#include <oledm/epaper.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus16x32.h>
#include <util/delay.h>

#define OLEDM_INIT oledm_basic_init

struct OLEDM display;
struct Text text;

int main(void) {
  oledm_basic_init(&display);
  //display.option_bits |= OLEDM_ROTATE_180;
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  text_init(&text, terminus16x32, &display);
  text.row = 3;
  text.column = 40;
  text_str(&text, "Hello World!");
  epaper_swap_buffers(&display, SLEEP_MODE_OFF);

  while (1);
}
