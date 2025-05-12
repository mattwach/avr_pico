#include <oledm/oledm.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

// Pin connections to Arduino Nano
// D8 -> RES (reset)
// D9 -> DC (data/command)
// D10 -> CS (chip select)
// D11 -> SDA (MOSI)
// D13 -> SCL (SCK)

struct OLEDM display;

const uint8_t hi[] PROGMEM = {
    0x00,
    0x10,
    0x20,
    0x40,
    0xFF,
};

static void set_colors(struct OLEDM* display, uint8_t counter) {
  uint16_t fg = 0;
  if (counter & 0x01) {
    fg |= 0b0000000000011111;  // Blue
  }
  if (counter & 0x02) {
    fg |= 0b0000011111100000;  // Green
  }
  if (counter & 0x04) {
    fg |= 0b1111100000000000;  // Red
  }
  if (counter & 0x08) {
    fg >>= 1;
  }
  display->fg_color = fg;
  uint16_t bg = 0;
  if (counter & 0x10) {
    bg |= 0b0000000000000111;  // Blue
  }
  if (counter & 0x20) {
    bg |= 0b0000000111100000;  // Green
  }
  if (counter & 0x40) {
    bg |= 0b0011100000000000;  // Red
  }
  if (counter & 0x80) {
    bg >>= 1;
  }
  display->bg_color = bg;
}

int main(void) {
  oledm_basic_init(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  uint8_t counter = 0;
  while (1) {
    ++counter;
    if (counter == 0) {
      counter = 1;
    }
    set_colors(&display, counter);
    _delay_ms(250);
    oledm_start_pixels(&display);
    uint8_t i = 0;
    for (; i < sizeof(hi); ++i) {
      oledm_write_pixels(&display, pgm_read_byte_near(hi + i));
    }
    oledm_stop(&display);
  }
}
