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
#include <pstr/pstr.h>
#include <rf24/rf24.h>
#include <util/delay.h>

#define DPIN (1 << 3)
#define ADC_CHANNEL 7

#define SAMPLE_DELAY SLEEP_8S
#define DELAY_ITERATIONS 37
//#define DELAY_ITERATIONS 1
#define SAMPLE_INTERVAL_MS 15

const uint8_t address[6] = "Yach0W";
volatile int16_t num_sent;
RF24 radio;

uint16_t sample(void) {
  PORTD |= DPIN;  // Turn On
  _delay_ms(SAMPLE_INTERVAL_MS); // settle
  uint16_t val = adc_read16(ADC_CHANNEL, ADC_REF_AVCC, ADC_PRESCALER_128);
  PORTD &= ~DPIN;  // Turn Off
  return val;
}


void delay(void) {
  for (int i=0; i < DELAY_ITERATIONS; ++i) {
    lowpower_powerDown(SAMPLE_DELAY, ADC_OFF, BOD_OFF);
  }
}


bool_t init_radio(void) {
  rf24_init(&radio, 9, 10); // CE, CSN
  if (!rf24_begin(&radio)) {
    return FALSE;
  }
  rf24_enable_dynamic_payloads(&radio);
  rf24_set_data_rate(&radio, RF24_250KBPS);
  rf24_set_pa_level(&radio, RF24_PA_HIGH);
  rf24_open_writing_pipe(&radio, address);
  rf24_stop_listening(&radio);
  return TRUE;
}


void send(uint16_t val) {
  rf24_power_up(&radio);
  uint8_t s[64];
  s[0] = 0;
  cs_to_ps_cpy(s, "SOIL 0 ");
  pstrcat(s, u16_to_ps(val));
  cs_to_ps_cat(s, " | ");
  pstrcat(s, u16_to_ps(++num_sent)); 
  pstrappend(s, '\n');
  rf24_write(&radio, ps_to_cs(s), s[0], FALSE);
  rf24_power_down(&radio);
}


int main(void) {
  num_sent = 0;
  sei();
  DDRD |= DPIN;  // D3 as output
  PORTD &= ~DPIN;  // set to low for now

  init_radio();

  while (1) {
    uint16_t val = sample(); 
	send(val);
	delay();
  }
}
