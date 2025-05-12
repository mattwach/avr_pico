#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#define PIN 5

int main(void) {
  DDRA = 1 << PIN;
  while (1) {
    PORTA = 1 << PIN;
    _delay_ms(1000);
    PORTA = ~(1 << PIN);
    _delay_ms(100);
  }
}
