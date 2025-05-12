#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#define PIN 3

int main(void) {
  DDRB = 1 << PIN;
  while (1) {
    PORTB = 1 << PIN;
    _delay_ms(1000);
    PORTB = ~(1 << PIN);
    _delay_ms(100);
  }
}
