#include <lowpower/lowpower.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus6x8.h>
#include <pstr/pstr.h>

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

uint16_t count = 0;

static void draw_count() {
  for (text.row = 0; text.row < 8; ++text.row) {
    text.column = 0;
    text_pstr(&text, u16_to_ps(count++));
  }
}

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  text_init(&text, terminus6x8, &display);

  oledm_clear(&display, 0x00);

  draw_count();
  while (1) {
    lowpower_powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
    oledm_display_on(&display);
    draw_count();
    lowpower_powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
    oledm_display_off(&display);
  }
}
