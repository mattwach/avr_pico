
#include <avr/io.h>
#include <ds3231/ds3231.h>
#include <lowpower/lowpower.h>
#include <pstr/pstr.h>
#include <uart/uart.h>

struct DS3231 ds3231;

char *days_of_week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

static void read_time() {
  ds3231_read(&ds3231);
  if (ds3231.error) {
    uart_str("Error: 0x");
    uart_pstr(u8_to_pshex(ds3231.error));
    uart_newln();
    lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
 
  uart_str(days_of_week[ds3231_parse_day_of_week(&ds3231)]);
  uart_str(" ");
  uart_pstr(u8_to_ps(ds3231_parse_year(&ds3231)));
  uart_str("-");
  uart_pstr(u8_to_ps(ds3231_parse_month(&ds3231)));
  uart_str("-");
  uart_pstr(u8_to_ps(ds3231_parse_day_of_month(&ds3231)));
  uart_str(" ");
  uart_pstr(u8_to_ps(ds3231_parse_hours_24(&ds3231)));
  uart_str(":");
  uart_pstr(u8_to_ps(ds3231_parse_minutes(&ds3231)));
  uart_str(":");
  uart_pstr(u8_to_ps(ds3231_parse_seconds(&ds3231)));
  uart_newln();
}

int main(void) {
  uart_init(9600, ENABLE_TX);

  uart_strln("Initializing DS3231.");
  ds3231_init(&ds3231);

  while (1) {
    read_time();
    lowpower_powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
}
