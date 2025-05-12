
#include <avr/io.h>
#include <oledm/oledm.h>
#include <oledm/ssd1780_lowlevel.h>
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

  ssd1780_start();
  ssd1780_fill_enable(RECTANGLE_FILL_ENABLE);
  ssd1780_draw_rect(0, 0, 95, 63, 0xFFFF, rgb16(0, 0, 10));
  const uint8_t x1 = 50;
  const uint8_t y1 = 5;
  const uint8_t x2 = 90;
  const uint8_t y2 = 60;
  const uint8_t x3 = 10;
  const uint8_t y3 = 45;
  const uint16_t color = rgb16(31, 63, 0);
  ssd1780_draw_line(x1, y1, x2, y2, color);
  ssd1780_draw_line(x2, y2, x3, y3, color);
  ssd1780_draw_line(x3, y3, x1, y1, color);
  ssd1780_stop();

  while (1) {
    _delay_ms(500);
    PORTD &= ~(1 << LED_PIN);
    _delay_ms(500);
    PORTD |= (1 << LED_PIN);
  }
}
