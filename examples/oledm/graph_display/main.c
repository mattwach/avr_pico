#include <adc/adc.h>
#include <data/stream_u16_to_u8.h>
#include <oledm/graph_display.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus6x8.h>
#include <pstr/pstr.h>
// Connect ADC to a pot and move it to manipulate a scrolling graph

#define ADC_CHANNEL 6  // A6

#define OLEDM_INIT oledm_basic_init

struct OLEDM display;
struct GraphDisplay gd;
struct StreamU16ToU8 stream;
struct Text text;

uint16_t adc_data[128];
uint8_t graph_data[128];

void timer_init() {
  // targeting 100ms per overflow
  // with / 1024 we get inv(16e6) * 1024 * 1e6 -> 64us per step
  // 100000 us / 64 us -> 1562.  Se we'll go with the 16-bit timer 1
  TCCR1B = 0x05 | 0x08;  // divide clock by 1024 | WGM12 (count to OCR1A)
  OCR1A = 1562;
}

uint8_t every10(column_t column) {
  return (column % 10) == 0 ? 3 : 0;
}

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  text_init(&text, terminus6x8, &display) ;
  graph_display_init(&gd, &display, 128, 7, graph_data);
  gd.ticmark_callback = every10;
  stream_u16_to_u8_init(&stream, 128, 7 * 8, adc_data, graph_data);
  timer_init();

  while (1) {
    // take an adc reading
    const uint16_t adc_value = adc_read16(
        ADC_CHANNEL, ADC_REF_AVCC, ADC_PRESCALER_128);

    stream_u16_to_u8_add_point(&stream, adc_value);
    gd.column_offset = stream.head;
    graph_display_render(&gd, 0, 0);

    text.row = 7;
    text.column = 0;
    text_pstr(&text, u16_to_ps(stream.min));
    text_char(&text, ' ');
    text_pstr(&text, u16_to_ps(adc_value));
    text_char(&text, ' ');
    text_pstr(&text, u16_to_ps(stream.max));
    text_char(&text, ' ');

    // wait for the timer
    while ((TIFR1 & (1 << OCF1A)) == 0);
    TIFR1 &= ~(1 << OCF1A);
  }
}
