
#include <uart/uart.h>
#include <uart/software_uart.h>
#include <pstr/pstr.h>
#include <avr/interrupt.h>

// A demo program
//
// 1) You connect RXD (PD0) to INT0 (PD2)
// 2) You load this software
// 3) You start something like minicom with 9600 baud
//
// Now you type a char in minicom:
// 1) It shows up on RXD and thus INT0
// 2) This demo program forwards it back to TXD
// 3) minicom shows you the ASCII value of what you typed (e.g. a -> 97)

void loop(void) {
  //uart_str("Hello World! ");
  //uart_pstrln(u32_to_ps(i));
  const SerialError last_err = software_uart_last_error();

  if (last_err != SERIAL_OK) {
    uart_str("err: ");
    uart_pstrln(u8_to_ps(last_err));
    return;
  }

  uint8_t c;
  if (software_uart_read(&c)) {
    uart_str("got: ");
    uart_pstrln(u8_to_ps(c));
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
