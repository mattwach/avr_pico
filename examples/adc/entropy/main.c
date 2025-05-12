// Take a number of ADC samples at different round counts and send the data
// to the UART.
//
// The idea to to process the data in ent and get formal randomness figures.
//
// The data looks like this:
//
// 5 AB
// 5 15
// Done 5 353455345
//
// Where the two column data is the round count and a 8-bit random number
// The Done count gives the rounds again and tells the number of milliseconds
// The process took.
//
// The process will proceed to the next round when MAX_SAMPLES or
// MAX_MILLISECONDS has occured.

#include <adc/adc.h>
#include <lowpower/lowpower.h>
#include <pstr/pstr.h>
#include <time/measure.h>
#include <uart/uart.h>
#include <util/delay.h>

#define MIN_ROUNDS 99
#define MAX_ROUNDS 100
#define MAX_SAMPLES 0xFFFF
#define MAX_MILLISECONDS 1800000 // #30 minutes
#define UART_BAUD 57600
#define ADC_CHANNEL 2

void count_down(void) {
  for (int i=10; i > 0; --i) {
    uart_pstr(u8_to_ps(i));
    uart_strln("...");
    _delay_ms(1000);
  }
}

void create_entropy(uint8_t rounds) {
  time_measure_reset();
  for (uint16_t sample = 0;
       (sample < MAX_SAMPLES) && (time_measure_ms() < MAX_MILLISECONDS);
       ++sample) {
    const uint8_t val = adc_entropy8(ADC_CHANNEL, 8, rounds);
    uart_pstr(u8_to_ps(rounds));
    uart_byte(' ');
    uart_pstrln(u8_to_pshex(val));
  }
  uart_str("Done ");
  uart_pstr(u8_to_ps(rounds));
  uart_byte(' ');
  uart_pstrln(u32_to_ps(time_measure_ms()));
}

int main(void) {
  uart_init(UART_BAUD, ENABLE_TX);
  count_down();
  time_measure_init();
  for (uint8_t rounds = MIN_ROUNDS; rounds <= MAX_ROUNDS; ++rounds) {
    create_entropy(rounds);
  }
  uart_strln("Finished");
  _delay_ms(1000);  
  lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}
