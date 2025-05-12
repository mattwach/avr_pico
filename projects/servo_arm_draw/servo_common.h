#ifndef SERVO_COMMON_H
#define SERVO_COMMON_H

#include <error_codes.h>

// Definition of slots

#define PEN_SERVO_SLOT 0  // needs to be zero, due to sequencing
#define LEFT_ARM_SERVO_SLOT 1
#define RIGHT_ARM_SERVO_SLOT 2

// Throttled seek setting
// We want full range in about 2000 ms
//
// full range is 2200 - 800 -> 1400 us in moment
//
// We will delay fo 15ms between commands, so 2000 / 15 -> 133 steps.
//
// So 1400 / 133 ~ 10us per 10ms step
#define THROTTLED_SEEK_US_PER_STEP 10

// This is what we load/store to EEPROM.
struct ServoCalibration {
  uint16_t left_low;
  uint16_t left_high;
  uint16_t right_low;
  uint16_t right_high;
  uint16_t pen_up;
  uint16_t pen_down;
  uint16_t checksum;
};


// Loads servo limits from EEPROM, initializes servos
struct ServoCalibration* servo_common_init(void);

// Saves current servo state to EEPROM
void servo_save_state(void);

// Gradually modify a servo from it's current value to an ending value
// This command is expected to be called in a UI loop
bool_t servo_throttled_seek(uint8_t slot, uint16_t target_us);

#endif
