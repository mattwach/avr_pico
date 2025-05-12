
#include <uart/uart.h>
#include <uart/software_uart.h>
#include <pstr/pstr.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// A demo program
//
// 1) You connect any external source @9600 baud to INT0 (PD2)
// 2) You load this software
// 3) You start something like minicom with 9600 baud
//
// minicom will show received characters.
// minicom will also show errors and pause after any error except BUFFER_FULL
// for 5 seconds.

void loop(void) {
  //uart_str("Hello World! ");
  //uart_pstrln(u32_to_ps(i));
  const SerialError last_err = software_uart_last_error();

  if (last_err != SERIAL_OK) {
    uart_str("err: ");
    uart_pstrln(u8_to_ps(last_err));
    if (last_err != SERIAL_ERROR_BUFFER_FULL) {
      _delay_ms(5000);
    }
    return;
  }

  uint8_t c;
  if (software_uart_read(&c)) {
    uart_byte(c);
  }
}

int main(void) {
  uart_init(9600, ENABLE_TX);
  software_uart_init(0);
  sei();
  uart_strln("Waiting for data");
  while (1) {
    loop();
  }
}
