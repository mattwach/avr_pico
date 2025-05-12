
// Wiring
//
//       +--------+
// N/C  -|        |- VDD
// LED2 -| Tiny85 |- SCL
// N/C  -|        |- LED1
// VSS  -|        |- SDA
//       +--------+
//
// Atmega328P:
//  D2 <- LED1
//  D3 <- LED2
//  A4 <- SDA
//  A5 <- SCL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <twi/twi.h>
#include <util/delay.h>

// Real time clock
#define ADDRESS 0x68

#ifdef __AVR_MEGA__
  #define LED_DDR DDRD
  #define LED_PORT PORTD
  #define LED1 (1 << 2)
#else
  #define LED_DDR DDRB
  #define LED_PORT PORTB
  #define LED1 (1 << 1)
#endif
#define LED2 (1 << 3)

void error(error_t err) {
  int i = 0;
  int code = err >> 4;
  for (; i < code; ++i) {
    LED_PORT |= LED2;
    _delay_ms(100);
    LED_PORT &= ~LED2;
    _delay_ms(100);
  }
  code = err & 0x0F;
  for (i=0; i < code; ++i) {
    LED_PORT |= LED1;
    _delay_ms(100);
    LED_PORT &= ~LED1;
    _delay_ms(100);
  }
		_delay_ms(1000);
}

void readClock(void) {
  uint8_t read_data[7];
  error_t err = 0;
  twi_startWrite(ADDRESS, &err);
  twi_writeNoStop(0, &err);
  twi_readWithStop(ADDRESS, read_data, sizeof(read_data), &err);
  if (err) {
    error(err);
  }
}

void loop(void) {
  while (1) {
    readClock();
    LED_PORT |= LED1;
    _delay_ms(499);
    LED_PORT &= ~LED1;
    _delay_ms(499);
  }
}

int main(void) {
  LED_DDR |= LED1 | LED2;  // Make the blinky light an output
  twi_init();
  loop();
}
