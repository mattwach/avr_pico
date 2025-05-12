
#include <port/port.h>
#include <debug/debug.h>
#include <pstr/pstr.h>
#include <uart/uart.h>
#include <weather/ms8607.h>
#include <util/delay.h>

struct MS8607 ms8607;

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
  port_set_all_input_pullup();
  uart_init(9600, ENABLE_TX);
  ms8607_init(&ms8607);
  while (1) {
    read_ms8607();
    _delay_ms(5000);
  }
}
