#include <stdint.h>
#include <lowpower/lowpower.h>
#include <port/port.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// This demo requires that the internal oscilator be used and that
// a 32K oscillator is connected between the external oscilator pins.

#define LED_PIN 5  // D5

static void timer_init(void) {
  ASSR = 0x20; // Enable the external 32k oscillator
  TCCR2B = 0x05; // Divide by 128 (for an overflow once per second)
  TIMSK2 = (1 << TOIE2);  // Interrupt on timer 2 overflow
  sei();  // enable global interrupts
}

int main(void) {
  DDRD = 1 << LED_PIN;
  timer_init();
  while (1) {
    lowpower_powerSave(SLEEP_FOREVER, ADC_OFF, BOD_OFF, TIMER2_ON);
  }
}

ISR(TIMER2_OVF_vect) {
  PORTD |= 1 << LED_PIN;
  _delay_ms(1);
  PORTD &= ~(1 << LED_PIN);
}
