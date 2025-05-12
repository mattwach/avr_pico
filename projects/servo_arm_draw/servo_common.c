
#include "servo_common.h"

#include <avr/eeprom.h>
#include <servo/servo_atmega.h>
#include <util/delay.h>

// Value limits
#define DEFAULT_LOW 1200
#define DEFAULT_HIGH 1800
#define DEFAULT_PEN_UP 1700
#define DEFAULT_PEN_DOWN 1500

#define LEFT_PIN 3
#define RIGHT_PIN 4
#define PEN_PIN 5


static struct ServoCalibration EEMEM e_servo_cal;  // EEPROM address
static struct ServoCalibration g_servo_cal;  // SRAM address

static uint16_t calc_checksum(void) {
  return
    0x0BAD +
    g_servo_cal.left_low +
    g_servo_cal.left_high +
    g_servo_cal.right_low +
    g_servo_cal.right_high;
}

struct ServoCalibration* servo_common_init(void) {
  eeprom_read_block(&g_servo_cal, &e_servo_cal, sizeof(struct ServoCalibration));
  if (calc_checksum() != g_servo_cal.checksum) {
    g_servo_cal.left_low = DEFAULT_LOW;
    g_servo_cal.left_high = DEFAULT_HIGH;
    g_servo_cal.right_low = DEFAULT_LOW;
    g_servo_cal.right_high = DEFAULT_HIGH;
    g_servo_cal.pen_down = DEFAULT_PEN_DOWN;
    g_servo_cal.pen_up = DEFAULT_PEN_UP;
  }

  servo_init_slot(PEN_SERVO_SLOT, 0, 0, 1 << PEN_PIN, g_servo_cal.pen_up);
  // Lets give the pen a bit of time to move up
  servo_set_count(1);
  _delay_ms(100);
  servo_init_slot(LEFT_ARM_SERVO_SLOT, 0, 0, 1 << LEFT_PIN, 1500);
  servo_init_slot(RIGHT_ARM_SERVO_SLOT, 0, 0, 1 << RIGHT_PIN, 1500);
  servo_set_count(3);

  return &g_servo_cal;
}

void servo_save_state(void) {
  g_servo_cal.checksum = calc_checksum();
  eeprom_write_block(&g_servo_cal, &e_servo_cal, sizeof(struct ServoCalibration));
}

bool_t servo_throttled_seek(uint8_t slot, uint16_t target_us) {
  const uint16_t current_us = servo_get_period_us(slot);
  if (current_us == target_us) {
    return TRUE;
  }

  int16_t delta = target_us - current_us;
  if (delta > THROTTLED_SEEK_US_PER_STEP) {
    delta = THROTTLED_SEEK_US_PER_STEP;
  } else if (delta < -THROTTLED_SEEK_US_PER_STEP) {
    delta = -THROTTLED_SEEK_US_PER_STEP;
  }

  servo_set_period_us(slot, current_us + delta);
  return FALSE;
}