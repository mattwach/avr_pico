
#include <avr/io.h>
#include <uart/uart.h>
#include <util/delay.h>
#include <pstr/pstr.h>
#include <port/port.h>

void loop(void) {
  uint32_t i = 0;
  while (1) {
    ++i;
    uart_str("Hello World! ");
    uart_pstrln(u32_to_ps(i));
  }
}

int main(void) {
  port_set_all_input_pullup();
  uart_init(9600, ENABLE_TX);
  loop();
}
