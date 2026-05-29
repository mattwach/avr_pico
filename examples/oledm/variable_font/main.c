#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <util/delay.h>
#include "hello_world_font.h"

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

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  text_init(&text, hello_world_font, &display);
  text_str(&text, "Hello World!");
}
