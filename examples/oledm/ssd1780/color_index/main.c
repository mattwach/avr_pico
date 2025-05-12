
#include <avr/io.h>
#include <oledm/oledm.h>
#include <util/delay.h>

// Pin connections to Arduino Nano
// D8 -> RES (reset)
// D9 -> DC (data/command)
// D10 -> CS (chip select)
// D11 -> SDA (MOSI)
// D13 -> SCL (SCK)

struct OLEDM display;

static void prepare(void) {
  oledm_clear(&display, 0x00);
  oledm_set_memory_bounds16(&display, 16, 0, 16 + 63, 63);
}

void color_grid(uint8_t colors_per_row) {
  prepare();
  const uint16_t span = 64 / colors_per_row;
  oledm_start_pixels(&display);
  for (uint16_t x=0; x < 64; ++x) {
    for (uint16_t y=0; y < 64; ++y) {
      const uint16_t color = rgb16_by_index(x / span + (y / span) * colors_per_row);
      oledm_write_pixel16(&display, color);
    }
  }

  oledm_stop(&display);
}

int main(void) {
  oledm_basic_init(&display);
  oledm_start(&display);

  while (1) {
    color_grid(4);
    _delay_ms(4000);
    color_grid(16);
    _delay_ms(4000);
  }
}
