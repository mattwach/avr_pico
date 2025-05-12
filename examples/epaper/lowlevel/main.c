#include <oledm/epaper.h>
#include <avr/pgmspace.h>
#include <uart/uart.h>
#include <util/delay.h>
#include <lowpower/lowpower.h>

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

static void init(void) {
  oledm_basic_init(&display);
//  display.option_bits |= OLEDM_ROTATE_180;
  oledm_start(&display);
}

static void loop(uint8_t clear_byte) {
  oledm_clear(&display, clear_byte);
  oledm_set_memory_bounds(
      &display, 8, 1, 255, display.memory_rows - 1);
  oledm_start_pixels(&display);
  for (uint8_t j=0; j < 200; ++j) {
    for (uint8_t i=0; i < sizeof(hi); ++i) {
      oledm_write_pixels(&display, pgm_read_byte_near(hi + i) ^ clear_byte);
    }
  }
  oledm_stop(&display);
  epaper_swap_buffers(&display, SLEEP_MODE_OFF);
}

int main(void) {
  init();
  uint8_t clear_byte = 0x00;
  while (1) {
    loop(clear_byte);
    lowpower_powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    clear_byte = clear_byte ^ 0xFF;
  }
}
