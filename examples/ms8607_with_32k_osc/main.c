#include <port/port.h>
#include <debug/debug.h>
#include <pstr/pstr.h>
#include <uart/uart.h>
#include <weather/ms8607.h>
#include <util/delay.h>
#include <lowpower/lowpower.h>

#include <avr/interrupt.h>
#include <avr/io.h>

struct MS8607 ms8607;

#define LED_PIN 5  // D5

static void timer_init(void) {
  ASSR = 0x20; // Enable the external 32k oscillator
  TCCR2B = 0x05; // Divide by 128 (for an overflow once per second)
  TIMSK2 = (1 << TOIE2);  // Interrupt on timer 2 overflow
  sei();  // enable global interrupts
}

static void dump_value(const char* header, int32_t v, const char* units) {
  uart_str(header);
  uart_byte(':');
  uart_byte(' ');
  if (v < 0) {
    uart_byte('-');
    v = -v;
  }
  uart_pstr(u32_to_ps(v / 100));
  uart_byte('.');
  const uint8_t r = (uint8_t)(v % 100);
  if (r < 10) {
    uart_byte('0');
  }
  uart_pstr(u8_to_ps(r));
  uart_strln(units);
}

static void read_ms8607(void) {
  int16_t temp_cc = 0;
  uint32_t pressure_pa = 0;
  uint16_t humidity_cpct = 0;
  ms8607_read_values(&ms8607, &temp_cc, &pressure_pa, &humidity_cpct);
  dump_value("Temperature", temp_cc, " C");
  dump_value("Pressure", pressure_pa, " hPa");
  dump_value("Humidity", humidity_cpct, " %");
}

int main(void) {
  //port_set_all_input_pullup();
  DDRD = 1 << LED_PIN;
  timer_init();
  uart_init(9600, ENABLE_TX);
  ms8607_init(&ms8607);
  while (1) {
    read_ms8607();
    //lowpower_powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    //_delay_ms(1000);
    lowpower_powerSave(SLEEP_4S, ADC_OFF, BOD_OFF, TIMER2_ON);
    //TWCR &= ~((1 << TWSTO) || (1 << TWEN));
    //TWCR |= (1 << TWEN);
    //DDRC |= ((1 << 4) | (1 << 5));
    //PORTC &= ~((1 << 4) | (1 << 5));
    //twi_reinit();
    //ms8607_init(&ms8607);
  }
}

ISR(TIMER2_OVF_vect) {
  PORTD |= 1 << LED_PIN;
  _delay_ms(1);
  PORTD &= ~(1 << LED_PIN);
}
