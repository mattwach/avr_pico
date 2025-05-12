#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#define PIN 0
#define MASK (1 << 2)

int main(void) {
  DDRB = 0x02;  // make port 1 an output
		PORTB = MASK; // make everything a pullup
  while (1) {
    if ((PINB & MASK) == MASK) {
						PORTB &= ~0x02;
				} else {
						PORTB |= 0x02;
				}
    _delay_ms(10);
  }
}
