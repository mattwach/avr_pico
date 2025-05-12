#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <lowpower/lowpower.h>

#ifdef __AVR_MEGA__
  #define LED_DDR DDRD
  #define LED_PORT PORTD
  #define LED1 (1 << 2)
#else
  #define LED_DDR DDRB
  #define LED_PORT PORTB
  #define LED1 (1 << 1)
#endif

int main(void) {
  LED_DDR = LED1;  // Make the port an output
		sei();
		while (1) {
				LED_PORT = LED1;
				lowpower_powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
				LED_PORT = 0x00;
				lowpower_powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
		}
}
