#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus8x16.h>
#include <time/measure.h>
#include <util/delay.h>

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
//#define OLEDM_INIT ssd1306_128x32_a_init

struct OLEDM display;
struct Text text;

static inline void calc_hours(uint32_t ms, uint8_t* out) {
  uint8_t hours = (ms / 3600000) % 100;
  out[0] = '0' + hours / 10;
  out[1] = '0' + hours % 10;
}

static inline void calc_mins(uint32_t ms, uint8_t* out) {
  uint8_t mins = (ms % 3600000) / 60000;
  out[0] = '0' + mins / 10;
  out[1] = '0' + mins % 10;
}

static inline void calc_secs(uint32_t ms, uint8_t* out) {
  uint8_t secs = (ms % 60000) / 1000;
  out[0] = '0' + secs / 10;
  out[1] = '0' + secs % 10;
}

static inline void calc_ms(uint32_t ms, uint8_t* out) {
  out[0] = '0' + ((ms / 100) % 10);
  out[1] = '0' + ((ms / 10) % 10);
  out[2] = '0' + ms % 10;
}

void show_time(uint32_t ms, uint8_t row) {
  // column count will be 128 / 8 = 16
  // Format will thus be:
  // HH:MM:SS.000
  // 123456789ABC  (12)
  uint8_t pstr[13];
  pstr[0] = 12;

  calc_hours(ms, pstr + 1);  // pstr[1], pstr[2]
  pstr[3] = ':';
  calc_mins(ms, pstr + 4);  // pstr[4], pstr[5]
  pstr[6] = ':';
  calc_secs(ms, pstr + 7); // pstr[7], pstr[8]
  pstr[9] = '.';
  calc_ms(ms, pstr + 10); // pstr[10], pstr[11], pstr[12]
  
  text.row = row;
  text.column = 16; // (128 - (12 * 8)) / 2 = (128 - 96) / 2 = 16
  text_pstr(&text, pstr);
}

void count_down(void) {
  uint8_t pstr[4];
  pstr[0] = 3;
  pstr[2] = '.';
  for (int8_t i=50; i > 0; --i) {
    pstr[1] = '0' + i / 10;
    pstr[3] = '0' + i % 10;
    text.row = (display.visible_rows >> 1) - 1;
    text.column = 52;
    text_pstr(&text, pstr);
    _delay_ms(100);
  }
}

int main(void) {
  OLEDM_INIT(&display);
  text_init(&text, terminus8x16, &display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  count_down();

  time_measure_init();
  while (1) {
    show_time(time_measure_ms(), (display.visible_rows >> 1) - 1);
    _delay_us(15);
  }
}
