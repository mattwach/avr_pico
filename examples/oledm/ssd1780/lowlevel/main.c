
#include <avr/io.h>
#include <oledm/oledm.h>
#include <util/delay.h>

// Pin connections to Arduino Nano
// D8 -> RES (reset)
// D9 -> DC (data/command)
// D10 -> CS (chip select)
// D11 -> SDA (MOSI)
// D13 -> SCL (SCK)

#define LED_PIN 5  // D5 Can't use built in LED because it's used by SPI

struct OLEDM display;

int main(void) {
  DDRD |= (1 << LED_PIN);
  PORTD |= (1 << LED_PIN);
  oledm_basic_init(&display);
  oledm_start(&display);
  oledm_start_pixels(&display);

  // Upper left will be r=0, b=31, g=0 -> blue
  // Lower left will be r=31, b=0, g=0 -> red
  // Upper right will be r=0, b=31, g=63 -> cyan
  // Lower right will be r=31, b=0, g=63 -> yellow
  for (uint16_t x=0; x < 96; ++x) {
    for (uint16_t y=0; y < 64; ++y) {
      const uint16_t red = y / 2;
      const uint16_t blue = 31 - red;
      const uint16_t green = x * 63 / 95;
      oledm_write_pixel16(
          &display,
          (red << 11) | (green << 5) | blue
      );
    }
  }

  oledm_stop(&display);
  while (1) {
    _delay_ms(500);
    PORTD &= ~(1 << LED_PIN);
    _delay_ms(500);
    PORTD |= (1 << LED_PIN);
  }
}
