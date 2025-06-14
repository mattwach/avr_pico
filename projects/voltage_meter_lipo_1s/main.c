// Measures voltage at VDD pin and outputs it to a 128x64 OLED
//
// Wiring
//
//         +--------+
// N/C    -|        |- VDD
// CAL    -| Tiny85 |- SCL for display
// N/C    -|        |- N/C 
// VSS    -|        |- SDA for display
//         +--------+
//
// Calibration instructions:
//
// 1. Set voltage to 3v
// 2. Pull CAL pin (PB3) low to start clibration
// 3. Leave CAL grounded or not - either works
// 4. Set voltage to 4v
// 5. Do the opposite of #3 to complete / save calibrated values

#include <adc/adc.h>
#include <avr/io.h>
#include <avr/eeprom.h>

#include "calibration_font.h"
#include "large_font.h"

#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <util/delay.h>

// Pin Definitions

#define SDA_PIN PB0
#define SCL_PIN PB2
#define CAL_PIN PB3

// Expect:
//   3v reading to be about 1100 * 1023 / 3000 = 375
//   3v reading to be about 1100 * 1023 / 4000 = 281
// that is a gap of 375 - 381 = 94
// well use a gap of 45 for min calibration span
#define MIN_ADC_CALIBRATION_SPAN 45

// Defines eeprom layout
struct SavedState {
  uint32_t vd_mv;   // Voltage drop in mv
  uint32_t vbg_mv;  // Bandgap voltage in mv
  uint32_t eyecatcher;
};

#define EYECATCHER 0xA85ADC00

// Global structures

struct OLEDM display;
struct Text text;
struct SavedState EEMEM saved_state_eeprom;  // EEPROM address
struct SavedState saved_state;

#define ADC_SAMPLES 8
uint16_t adc_sample[ADC_SAMPLES];
uint8_t adc_sample_index;
uint32_t adc_sample_sum;

// Calculates a voltage Drop (Vd) in mv, given an ADC sample at 3v and 4v
static inline uint32_t vd_mv(uint32_t adc_3v, uint32_t adc_4v) {
  return (3000 * adc_3v - 4000 * adc_4v) / (adc_3v - adc_4v);
}

// Calculates bandgap voltage (Vbg) in mv, given a 3v adc value and Vd
static inline uint32_t vbg_mv(uint32_t vd_mv, uint32_t adc_3v) {
  return (adc_3v * (3000 - vd_mv)) / 1023;
}

// Calculates a voltage, in mv, given and adc value, Vd and Vbg
static inline uint32_t voltage_mv(uint32_t adc) {
  return saved_state.vbg_mv * 1023 / adc + saved_state.vd_mv;
}

// returns 1 if the CAL pin is grounded
static inline uint8_t cal_is_grounded(void) {
  return (PINB & (1 << CAL_PIN)) == 0;
}

// Read eeprom value to saved_state and return 1 if the contents are valid
static uint8_t read_eeprom(void) {
  eeprom_read_block(
    &saved_state, &saved_state_eeprom, sizeof(struct SavedState));
  return saved_state.eyecatcher == EYECATCHER;
}

// Writes saved_state to eeprom.
static void write_eeprom(void) {
  saved_state.eyecatcher = EYECATCHER;
  eeprom_write_block(
    &saved_state, &saved_state_eeprom, sizeof(struct SavedState));
}

// Acquires ADC samples and averages them (digital low pass filter)
static void read_adc_sample(void) {
  adc_sample_sum -= adc_sample[adc_sample_index];
  adc_sample[adc_sample_index] = adc_quiet_read16_internal_ref(ADC_PRESCALER_128);
  adc_sample_sum += adc_sample[adc_sample_index];
  ++adc_sample_index;
  if (adc_sample_index >= ADC_SAMPLES) {
    adc_sample_index = 0;
  }
}

// Returns the filtered adc value
static inline uint32_t adc_current(void) {
  return adc_sample_sum / ADC_SAMPLES;
}

// shows a calibration line
static void show_calibration_line(const char* label, uint32_t val) {
  text_str(&text, label);
  text_char(&text, '|');
  text_char(&text, ' ');
  text_char(&text, '0' + (char)((val / 1000) % 10));
  text_char(&text, '0' + (char)((val / 100) % 10));
  text_char(&text, '0' + (char)((val / 10) % 10));
  text_char(&text, '0' + (char)(val % 10));
  text_char(&text, '\n');
}

// Shows current calibration values
static void print_adc_values_to_oled(uint32_t adc_3v, uint32_t adc_4v) {
  text.row = 2;
  text.column = 0;
  show_calibration_line("adc 3v", adc_3v);
  show_calibration_line("adc 4v", adc_4v);
}

// Shows finished calibration state
static void print_cal_values_to_oled(uint32_t adc) {
  text.row = 0;
  text.column = 0;
  text_str(&text, "saved to eeprom\n");
  show_calibration_line("vbg mv", saved_state.vbg_mv);
  show_calibration_line(" vd mv", saved_state.vd_mv);
  show_calibration_line("  v mv", voltage_mv(adc));
}

// Calibrates Vgs and Vb
static void calibrate(void) {
  oledm_clear(&display, 0x00);
  uint32_t adc_3v = adc_current();
  uint32_t adc_4v = 0;
  uint8_t grounded = 1;
  while (1) {
    read_adc_sample();
    const uint32_t adc = adc_current();

    if (adc_4v == 0) {
      if ((adc + MIN_ADC_CALIBRATION_SPAN) > adc_3v) {
        // not enough gap.  Set the grounded state to the current state
        // so that either a button press or a GND release will work
        grounded = cal_is_grounded(); 
      } else if (cal_is_grounded() != grounded) {
        adc_4v = adc;
        saved_state.vd_mv = vd_mv(adc_3v, adc_4v);
        saved_state.vbg_mv = vbg_mv(saved_state.vd_mv, adc_3v);
        write_eeprom();
        oledm_clear(&display, 0x00);
      }
      print_adc_values_to_oled(adc_3v, adc);
    } else {
      print_cal_values_to_oled(adc);
    }
  }
}

// One-time initilization that occurs when the chip is given power
static void init(void) {
  // settle time
  _delay_ms(50);

  // Set everything as an input to start
  DDRB = 0x00;
  // Set internal pullups for input pins
  PORTB = (1 << PB1) | (1 << PB3) | (1 << PB4);
  // Set SDA and SCL as outputs
  DDRB |= (1 << SDA_PIN) | (1 << SCL_PIN);

  // Initialize the OLED
  oledm_basic_init(&display);
  text_init(&text, large_font, &display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  if (!read_eeprom()) {
    saved_state.vd_mv = 0;
    saved_state.vbg_mv = 0;
    text.font = calibration_font;
  }
}

// Outputs a large voltage readout to the display. e.g. 3.78v
static void print_voltage_to_oled(uint32_t mv) {
  text.row = 0;
  text.column = 0;
  text_char(&text, '0' + (char)(mv / 1000));
  text_char(&text, '.');
  text_char(&text, '0' + (char)((mv / 100) % 10));
  text_char(&text, '0' + (char)((mv / 10) % 10));
  text_char(&text, 'v');
}

// Outputs that the voltage is not known
static void calibrate_message(void) {
  text.row = 1;
  text.column = 0;
  text_str(&text,
      "Set 3v, then\n"
      "pull CAL to\n"
      "GND.\n");
}

// Called repeatedly by main
static void loop(void) {
  read_adc_sample();
  if (cal_is_grounded()) {
    calibrate();
  }
  if (saved_state.vd_mv != 0) {
    print_voltage_to_oled(voltage_mv(adc_current()));
  }
}


// Program entry point
int main(void) {
  init();
  if (saved_state.vd_mv == 0) {
    calibrate_message();
  }
  while (1) {
    loop();
  }
}

