#ifndef GIMBAL_H
#define GIMBAL_H
// Provides functions for managing the state of a gimbal.

#include <inttypes.h>

#define GIMBAL_CALIBRATE_SAMPLES 16
#define GIMBAL_CALIBRATE_MAX 0x3ff
#define GIMBAL_CALIBRATE_CENTER 0x200

// GimbalCalibrate is used to automatically final the center and
// endpoints of the gimbal pot.  Everything is converted to
// calibrated units as defined by GIMBAL_CALIBRATE_MAX and
// GIMBAL_CALIBRATE_CENTER.
struct GimbalCalibrate {
  uint16_t adc_center;
  uint16_t adc_min;
  uint16_t adc_max;
  uint16_t adc_center_sample[GIMBAL_CALIBRATE_SAMPLES];
  uint16_t num_samples;
};

// Initialize calibration.
//
// adc_min and adc_max will be auto-expanded as gimbal_calibrate() is fed
// values pick something consertatively low here.  If you have good
// precalibrated numbers, they can be fed or something conservative can be fed
// in (for example, 700-3300 in a 10 bit system) and let the expansion happen
// dynamically.
//
// adc_center is determines after N (currently 16) calls to gimbal_calibrate()
void gimbal_cal_init(
    struct GimbalCalibrate* gc,
    uint16_t adc_min,
    uint16_t adc_max);

// Gets the current value and updates internal structures.  Value returned
// will be in the range of 0-1023.
uint16_t gimbal_calibrate(struct GimbalCalibrate* ga, uint16_t adc_value);


// GimbalDirection is used to convert analog movements of the gimbal to
// directional intent - like choosing a menu item.  This version does not
// support scrolling but does support moving the gimbal forward and back
// with a bias without needing to return to center.
struct GimbalDirection {
  // how much deadzone to give to center readings
  uint16_t deadzone;
  // the user can push the stick up, pull back a little (not all the way to
  // center), then go up again.  The pull back amount is defined here
  uint16_t pullback;
  // tracks current state
  uint8_t state;
  // used to help state transitions
  uint16_t anchor;
};

// Initialize direction.  Note that units are in post-calibrated units (0-1023)
//
// deadzone is used for direction (determine if the operator is actively
// driving the gimbal)
//
// pullback allows the user to step options without returning all the way to
// the center.
void gimbal_dir_init(
    struct GimbalDirection* gd,
    uint16_t deadzone,
    uint16_t pullback);

// This is primarily used for selecting previous and next values
// in menus.  e.g., if someone holds up, they don't want blazing
// 60fps selection scrolling.  This algorithm instead follows the
// pattern of "back-then-forward" suggests an intent to move
// forward.
//
// Pass the value returned from gimbal_calibrate().  This is done
// so that the converted value can be captured without extra calls
// to gimbal_calibrate() (as early calls affect the centering
// calculation).
//
// Returns -1 (back one step), 0 (no change), or 1 (forward one step)
int8_t gimbal_direction(struct GimbalDirection* gd, uint16_t calv);

#endif

