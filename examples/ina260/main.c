
#include <avr/io.h>
#include <ina260/ina260.h>
#include <lowpower/lowpower.h>
#include <pstr/pstr.h>
#include <uart/uart.h>
#include <util/delay.h>

#define I2C_ADDRESS 0x40
// uncomment this to enter/exit low power mode
// so that current to INA260 can be measured with
// a multimeter.
//#define TEST_LOW_POWER_MODE

struct INA260 ina260_g;

char *averaging_modes[] = {
  "AVG_1_SAMPLE",
  "AVG_4_SAMPLES",
  "AVG_16_SAMPLES",
  "AVG_64_SAMPLES",
  "AVG_128_SAMPLES",
  "AVG_256_SAMPLES",
  "AVG_512_SAMPLES",
  "AVG_1024_SAMPLES"
};

char *conversion_times[] = {
  "CONVERSION_TIME_140_US",
  "CONVERSION_TIME_204_US",
  "CONVERSION_TIME_332_US",
  "CONVERSION_TIME_588_US",
  "CONVERSION_TIME_1100_US",
  "CONVERSION_TIME_2116_US",
  "CONVERSION_TIME_4156_US",
  "CONVERSION_TIME_8244_US"
};

char *operating_modes[] = {
  "OPERATING_MODE_POWER_DOWN",
  "OPERATING_MODE_TRIGGERED_CURRENT",
  "OPERATING_MODE_TRIGGERED_VOLTAGE",
  "OPERATING_MODE_TRIGGERED_CURRENT_AND_VOLTAGE",
  "OPERATING_MODE_POWER_DOWN2",
  "OPERATING_MODE_CONTINUOUS_CURRENT",
  "OPERATING_MODE_CONTINUOUS_VOLTAGE",
  "OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE"
};

static void terminate(const char* msg) {
  uart_strln(msg);
  while (1) {
    lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}

static uint16_t read_config(void) {
  uart_strln("Reading configuration");
  const uint16_t configuration = ina260_read_configuration(&ina260_g);
  uart_str("  raw value: 0x");
  uart_pstrln(u16_to_pshex(configuration));

  uart_str("  averaging mode: ");
  uart_strln(averaging_modes[ina260_parse_averaging_mode(configuration)]);

  uart_str("  voltage conversion time: ");
  uart_strln(conversion_times[ina260_parse_voltage_conversion_time(configuration)]);

  uart_str("  current conversion time: ");
  uart_strln(conversion_times[ina260_parse_current_conversion_time(configuration)]);

  uart_str("\n\r  operating mode: ");
  uart_strln(operating_modes[ina260_parse_operating_mode(configuration)]);

  return configuration;
}

static void write_config(uint16_t config) {
  uart_str("Writing new config: 0x");
  uart_pstrln(u16_to_pshex(config));
  ina260_write_configuration(&ina260_g, config);
  uart_strln("Wrote config");
}

static void init() {
  uart_init(9600, ENABLE_TX);

  uart_strln("Initializing INA260.");
  ina260_init(&ina260_g, I2C_ADDRESS);

  const uint16_t config = ina260_create_configuration(
      AVG_16_SAMPLES,
      CONVERSION_TIME_8244_US,
      CONVERSION_TIME_8244_US,
      OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE);
  uint16_t current_config = read_config();
  if (config == current_config) {
    uart_strln("Config is already set as needed.");
  } else {
    write_config(config);
    current_config = read_config();
    if (config != current_config) {
      terminate("Failed to set config.");
    }
  }
}

static void take_measurement(void) {
  // check for any errors
  if (ina260_g.error) {
    uart_str("INA260/I2C error: 0x");
    uart_pstrln(u8_to_pshex(ina260_g.error));
    terminate("Aborting.");
  }

  uart_str("Measurement: ");
  uart_pstr(u16_to_ps(ina260_read_voltage_in_mv(&ina260_g)));
  uart_str(" mV  ");
  uart_pstr(i16_to_ps(ina260_read_current_in_ma(&ina260_g)));
  uart_str(" mA  ");
  uart_pstr(u16_to_ps(ina260_read_power_in_mw(&ina260_g)));
  uart_strln(" mW");
}

#ifdef TEST_LOW_POWER_MODE
static void low_power_wait(void) {
  uart_strln("Enter low power mode");
  uart_wait();
  ina260_set_operating_mode(&ina260_g, OPERATING_MODE_POWER_DOWN2); 
  lowpower_powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  ina260_set_operating_mode(&ina260_g, OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE); 
  uart_strln("Exit low power mode");
}
#endif

int main(void) {
  init();

  while (1) {
    take_measurement();
#ifdef TEST_LOW_POWER_MODE
    low_power_wait();
#endif
    _delay_ms(2000);  // Give some time to validate the number
  }

  terminate("Power down.");
}
