#include <oledm/font/terminus16x32_numbers.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <servo/read_servo_pwm.h>
#include <util/delay.h>

// Wiring
//
// Atmega328P:
//  A4 <- SDA
//  A5 <- SCL

struct OLEDM display;
struct Text text_16x32;

static void init() {
  _delay_ms(50);  // the oled needs a little time to settle
  oledm_basic_init(&display);
  text_init(&text_16x32, terminus16x32_numbers, &display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  read_servo_pwm_init(150);  
}

static void update_state() {
	// just use a static number for the first pass
	const uint16_t val = read_servo_pwm();
	text_16x32.row = 1;
	text_16x32.column = 16; // center the output
	text_char(&text_16x32, '0' + val / 10000);
	text_char(&text_16x32, '0' + (val / 1000) % 10);
	text_char(&text_16x32, '0' + (val / 100) % 10);
	text_char(&text_16x32, '0' + (val / 10) % 10);
	text_char(&text_16x32, '0' + val % 10);
}

int main(void) {
  init();

  while (1) {
    _delay_ms(50);
    update_state();
  }
}

