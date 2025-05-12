
#include <avr/io.h>
#include <ina260/ina260.h>
#include <lowpower/lowpower.h>
#include <oledm/console.h>
#include <oledm/font/terminus6x8.h>
#include <oledm/oledm.h>
#include <oledm/ssd1306_init.h>
#include <pstr/pstr.h>
#include <uart/uart.h>
#include <util/delay.h>

#define I2C_ADDRESS 0x40
// uncomment this to enter/exit low power mode
// so that current to INA260 can be measured with
// a multimeter.
//#define TEST_LOW_POWER_MODE

struct INA260 ina260_g;
struct Console console;
struct OLEDM display;

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

static void println(const char* msg) {
  uart_strln(msg);
  console_print(&console, msg);
  console_print(&console, "\n");
}

static void print(const char* msg) {
  uart_str(msg);
  console_print(&console, msg);
}

static void pprint(const uint8_t* msg) {
  uart_pstr(msg);
  console_pprint(&console, msg);
}

static void pprintln(const uint8_t* msg) {
  uart_pstrln(msg);
  console_pprint(&console, msg);
  console_print(&console, "\n");
}

static void terminate(const char* msg) {
  println(msg);
  while (1) {
    lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}

static uint16_t read_config(void) {
  println("Reading configuration");
  const uint16_t configuration = ina260_read_configuration(&ina260_g);
  print("  raw value: 0x");
  pprintln(u16_to_pshex(configuration));

  print("  averaging mode: ");
  println(averaging_modes[ina260_parse_averaging_mode(configuration)]);

  print("  voltage conversion time: ");
  println(conversion_times[ina260_parse_voltage_conversion_time(configuration)]);

  print("  current conversion time: ");
  println(conversion_times[ina260_parse_current_conversion_time(configuration)]);

  print("\n\r  operating mode: ");
  println(operating_modes[ina260_parse_operating_mode(configuration)]);

  return configuration;
}

static void write_config(uint16_t config) {
  print("Writing new config: 0x");
  pprintln(u16_to_pshex(config));
  ina260_write_configuration(&ina260_g, config);
  println("Wrote config");
}

static void init() {
  uart_init(9600, ENABLE_TX);
  oledm_basic_init(&display);
  oledm_start(&display);

  oledm_clear(&display, 0x00);
  console_init(&console, &display, terminus6x8);

  println("Initialing INA260.");
  ina260_init(&ina260_g, I2C_ADDRESS);

  const uint16_t config = ina260_create_configuration(
      AVG_16_SAMPLES,
      CONVERSION_TIME_8244_US,
      CONVERSION_TIME_8244_US,
      OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE);
  uint16_t current_config = read_config();
  if (config == current_config) {
    println("Config is already set as needed.");
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
    print("INA260/I2C error: 0x");
    pprintln(u8_to_pshex(ina260_g.error));
    terminate("Aborting.");
  }

  pprint(u16_to_ps(ina260_read_voltage_in_mv(&ina260_g)));
  print(" mV  ");
  pprint(i16_to_ps(ina260_read_current_in_ma(&ina260_g)));
  print(" mA  ");
  pprint(u16_to_ps(ina260_read_power_in_mw(&ina260_g)));
  print(" mW ");
  print(" IE");
  pprint(u8_to_pshex(ina260_g.error));
  print(" OE");
  pprintln(u8_to_pshex(display.error));
}

#ifdef TEST_LOW_POWER_MODE
static void low_power_wait(void) {
  println("Enter low power mode");
  uart_wait();
  ina260_set_operating_mode(&ina260_g, OPERATING_MODE_POWER_DOWN2); 
  lowpower_powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  ina260_set_operating_mode(&ina260_g, OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE); 
  println("Exit low power mode");
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
