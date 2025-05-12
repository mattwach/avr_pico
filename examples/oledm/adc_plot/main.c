#include <adc/adc.h>
#include <oledm/bitmap.h>
#include <oledm/oledm.h>
#include <oledm/ssd1306_init.h>
#include <lowpower/lowpower.h>
#include <avr/io.h>

//#define OLEDM_INIT ssd1306_64x32_a_init
//#define OLEDM_INIT ssd1306_128x32_a_init
#define OLEDM_INIT oledm_basic_init

struct OLEDM display;
struct Bitmap bitmap;
uint8_t bitmap_data[128 * 8];

#define POINTS_PER_FRAME 8
#define ADC_CHANNEL 2

void done(void) {
  const uint8_t w = bitmap.columns;
  const uint8_t h = bitmap.rows << 3;
  bitmap_rect(&bitmap, 0, 0, w, h, bitmap_NAND);
  bitmap_rect(&bitmap, 1, 1, w-2, h-2, bitmap_OR);
  bitmap_rect(&bitmap, 2, 2, w-4, h-4, bitmap_NAND);
  bitmap_render_fast(&display, &bitmap, 0, 0);
  lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);

  bitmap.rows = display.visible_rows;
  bitmap.columns = display.visible_columns;
  bitmap.data = bitmap_data;

  bitmap_fill(&bitmap, 0x00);
  bitmap_render_fast(&display, &bitmap, 0, 0);
  // Keep the ADC on
  ADCSRA = (1 << ADEN);

  uint8_t xbits = 1;
  for (; 1 << xbits < (display.visible_columns - 1); ++xbits);

  uint8_t ybits = 1;
  for (; 1 << ybits < (display.visible_rows - 1); ++ybits);
  ybits += 3;

  const uint16_t num_points = display.visible_rows * display.visible_columns * 8;

  for (uint16_t i=0; i < num_points; ++i) {
    uint8_t x = adc_entropy8(ADC_CHANNEL, xbits, 8);
    uint8_t y = adc_entropy8(ADC_CHANNEL, ybits, 8);
    bitmap_point(
        &bitmap,
        x,
        y,
        bitmap_OR
        );
    if ((i % POINTS_PER_FRAME) == 0) {
      bitmap_render_fast(&display, &bitmap, 0, 0);
    }
  }

  done();
}
