
#include <avr/io.h>
#include <ina260/ina260.h>
#include <lowpower/lowpower.h>
#include <pstr/pstr.h>
#include <uart/uart.h>
#include <util/delay.h>

#define I2C_ADDRESS 0x40
// Note, if you want slower than 1s, you'll need to change the divider ratio
// and recalculate a correct CLOCKS_PER_INTERVAL value.
#define POLL_INTERVAL_MS 1000

// 16,000,000 ticks ser second / 256 divider = 62500 clocks
// per second.  That's 62.5 clocks/ms
#define CLOCKS_PER_INTERVAL ((uint16_t)(((uint32_t)POLL_INTERVAL_MS * 62) + (POLL_INTERVAL_MS >> 1)))

struct INA260 ina260_g;
uint32_t uptime_ms;
uint16_t next_timer_val;

static void terminate(const char* msg) {
  uart_strln(msg);
  while (1) {
    lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}

static void init() {
  // Give the power a chance to stabilize
  _delay_ms(500);
  uart_init(9600, ENABLE_TX);

  uart_strln("Initializing INA260.");
  ina260_init(&ina260_g, I2C_ADDRESS);

  const uint16_t config = ina260_create_configuration(
      AVG_128_SAMPLES,
      CONVERSION_TIME_8244_US,
      CONVERSION_TIME_8244_US,
      OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE);
  ina260_write_configuration(&ina260_g, config);

  // Give the voltage meter a chance to stabilize too
  _delay_ms(500);

  // Start timer with /256 ratio
  TCCR1B = 0x04;
  next_timer_val = CLOCKS_PER_INTERVAL;
}

static void take_measurement(void) {
  // check for any errors
  if (ina260_g.error) {
    uart_str("INA260/I2C error: 0x");
    uart_pstrln(u8_to_pshex(ina260_g.error));
    terminate("Aborting.");
  }

  uart_pstr(u32_to_ps(uptime_ms));
  uart_str(" ms ");
  uart_pstr(u16_to_ps(ina260_read_voltage_in_mv(&ina260_g)));
  uart_str(" mV ");
  uart_pstr(i16_to_ps(ina260_read_current_in_ma(&ina260_g)));
  uart_strln(" mA");
}

int main(void) {
  init();

  while (1) {
    take_measurement();
    while (TCNT1 != next_timer_val);
    next_timer_val += CLOCKS_PER_INTERVAL;
  }

  // Should never reach this
  terminate("Power down.");
}
