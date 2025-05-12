// Simple setup for testing out the soil sensor code
//
// Hardware setup:
//
// This one uses the Nano with a digital pin, analog pin, and OLED display
//
// Periodically, the digital pin is raised and several measurements are taken and displayed
// in the OLED

#include <avr/io.h>
#include <adc/adc.h>
#include <avr/interrupt.h>
#include <lowpower/lowpower.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus8x16.h>
#include <pstr/pstr.h>
#include <util/delay.h>

#define DPIN (1 << 3)
#define ADC_CHANNEL 7

#define SAMPLE_DELAY SLEEP_8S
#define SAMPLE_INTERVAL_MS 15

#define OLEDM_INIT oledm_basic_init

struct OLEDM display;
struct Text text;

void sample() {
  oledm_clear(&display, 0x00);

  PORTD |= DPIN;  // Turn On

  for (int i=0; i < 8; ++i) {
    uint16_t val = adc_read16(ADC_CHANNEL, ADC_REF_AVCC, ADC_PRESCALER_128);
    
    text.row = (i & 3) * 2;
    text.column = (i < 4) ? 0 : 63;
    text_pstr(&text, u16_to_ps(val));

    _delay_ms(SAMPLE_INTERVAL_MS);
  }

  PORTD &= ~DPIN;  // Turn Off
}

int main(void) {
  sei();
  DDRD |= DPIN;  // D3 as output
  PORTD &= ~DPIN;  // set to low for now

  OLEDM_INIT(&display);
  oledm_start(&display);
  text_init(&text, terminus8x16, &display);

  while (1) {
    sample(); 
    lowpower_powerDown(SAMPLE_DELAY, ADC_OFF, BOD_OFF);
  }
}
