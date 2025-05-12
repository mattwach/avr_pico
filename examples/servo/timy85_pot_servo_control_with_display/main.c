// Uses a pot + 12c powered 64x32 display to control a servo.
// You can change the display to 128x64 with a small tweak to the
// code below...
//
// Wiring
//
//         +--------+
// N/C    -|        |- VDD
// POT IN -| Tiny85 |- SCL for display
// N/C    -|        |- Servo PWM
// VSS    -|        |- SDA for display
//         +--------+

#include <adc/adc.h>
#include <avr/io.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus16x32_numbers.h>
#include <pstr/pstr.h>
#include <servo/servo_attiny.h>
#include <time/measure.h>
#include <util/delay.h>

#define START_PWM 1500
#define MIN_PWM 988  // Minimum ADC value is 0. This is 1500 - 512
                     // Max PWM will then be 1500 + 512 -> 2012
#define SERVO_PIN PB1
#define ADC_PIN PB3
#define ROUNDING 100

struct OLEDM display;
struct Text text;

void update_servo() {
  uint16_t period_us = MIN_PWM + 
    adc_read16(ADC_PIN, ADC_REF_VCC, ADC_PRESCALER_128);

  period_us = ((period_us + (ROUNDING >> 1)) / ROUNDING) * ROUNDING; 

  servo_set(SERVO_PIN, period_us);

  text.row = 0;
  text.column = 0;
  const uint16_t real_period = servo_get_period_us();
  if (real_period < 1000) {
    text_strLen(&text, " ", 1);
  }
  text_pstr(&text, u16_to_ps(real_period));
}

int main(void) {
  ssd1306_64x32_a_init(&display);
  text_init(&text, terminus16x32_numbers, &display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  while (1) {
    update_servo();
    _delay_ms(10); // 100 updates / second maximum
  }
}
