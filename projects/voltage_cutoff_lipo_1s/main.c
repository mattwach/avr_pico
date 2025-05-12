// Monitors batteries from 1S-6S.
//
// Automatically cuts power when the battery gets below 3.6V / Cell
//
// Wiring
//
//          +--------+
// N/C     -|        |- VDD
// VOLTIN  -| Tiny85 |- N/C
// VOLTPWR -|        |- PWR 
// VSS     -|        |- CALIBRATE
//          +--------+
//
// VOLTIN is from a voltage divider.
//

#include <adc/adc.h>
#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <util/delay.h>

#define CAL_PIN PB0
#define PWR_PIN PB1
#define NC_PIN PB2
#define VOLTIN_PIN PB3
#define VOLTIN_DIDR 3  // maybe the same as PB3, not sure
#define VOLT_PWR_PIN PB4

// Using the 1.1V badgap reference, we are looking for 1V over
// a scale of 0-1023.  Thi results in a value of 1023 * 1 / 1.1 = 930
#define ADC_TRIGGER 930 
// How many back-to-back reading to read the voltage as low before
// triggering sleep.
#define READ_LOW_COUNT_TRIGGER 3

typedef uint8_t bool_t;

uint8_t read_low_count;

static inline void enable_load_power(void) {
  DDRB |= (1 << PWR_PIN);   // Set to output
  PORTB |= (1 << PWR_PIN);  // Turn on
}

static inline void disable_load_power(void) {
  PORTB &= ~(1 << PWR_PIN);  // Turn off
  DDRB &= ~(1 << PWR_PIN);   // Set to Hi-Z
}

static inline void enable_divider_power(void) {
  DDRB |= (1 << VOLT_PWR_PIN);   // Set to output
  PORTB |= (1 << VOLT_PWR_PIN);  // Turn on
}

static inline void disable_divider_power(void) {
  PORTB &= ~(1 << VOLT_PWR_PIN);  // Turn off
  DDRB &= ~(1 << VOLT_PWR_PIN);   // Set to Hi-Z
}

static inline uint16_t read_adc(void) {
  // Make sure the divider power is on or this wont work,
  return adc_quiet_read16(
      VOLTIN_PIN, ADC_REF_INTERNAL11, ADC_PRESCALER_128);
}

static bool_t voltage_is_ok(void) {
  enable_divider_power();
  // Let power levels settle
  lowpower_powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);

  // Do a "quiet" ADC reading
  const uint16_t raw_adc_value = read_adc();
  disable_divider_power();
  return raw_adc_value >= ADC_TRIGGER;
}

// returns 1 if the voltage is ok
static uint8_t check_voltage(void) {
  if (voltage_is_ok()) {
    // Reset this if it was incremented earlier
    read_low_count = 0;
    enable_load_power();
  } else {
    ++read_low_count;
    if (read_low_count >= READ_LOW_COUNT_TRIGGER) {
      --read_low_count;  // avoid overflow in the future
      disable_load_power();
      return 0;
    }
  }
  return 1;
}

static void monitor_voltage(void) {
  while (1) {
    if (check_voltage()) {
      lowpower_powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    } else {
      lowpower_powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
  }
}

static void calibrate_mode(void) {
  if (PINB & (1 << CAL_PIN)) {
    // Jumper is not added (pullup making pin high)
    return;
  }

  enable_divider_power();
  while ((PINB & (1 << CAL_PIN)) == 0) {
    if (read_adc() >= ADC_TRIGGER) {
      enable_load_power();
    } else {
      disable_load_power();
    }
    _delay_ms(20);  // 50 updates per second
  }
  disable_divider_power();
}

int main(void) {
  // Everything as a Hi-Z input is a good starting condition
  // NC_PIN and CAL_PIN use the internal pullup
  PORTB |= (1 << NC_PIN) | (1 << CAL_PIN);
  // Disable digital input reads on the ADC pin
  DIDR0 = (1 << VOLTIN_DIDR);

  // Give power caps a moment to charge up
  lowpower_powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF);

  read_low_count = 0;
  calibrate_mode();
  monitor_voltage();
}

