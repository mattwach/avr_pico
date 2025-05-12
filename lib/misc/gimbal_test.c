#include "gimbal.h"
#include "gimbal_state.h"

#include <test/unit_test.h>
#include <string.h>

void test_cal_init(void) {
  struct GimbalCalibrate gc;
  struct GimbalCalibrate expected_gc;
  memset(&expected_gc, 0, sizeof(struct GimbalCalibrate));
  expected_gc.adc_min = 34;
  expected_gc.adc_max = 3456;
  gimbal_cal_init(&gc, 34, 3456);
  assert_int_equal(
      0, memcmp(&expected_gc, &gc, sizeof(struct GimbalCalibrate)));
}

void test_convert_after_calibration(void) {
  struct GimbalCalibrate gc;
  gimbal_cal_init(&gc, 100, 3900);
  // set the center and mark it as calibrated so that we can focus on
  // conversions
  gc.adc_center = 2000;
  gc.num_samples = GIMBAL_CALIBRATE_SAMPLES;

  // center
  assert_int_equal(GIMBAL_CALIBRATE_CENTER, gimbal_calibrate(&gc, 2000));

  // minimum
  assert_int_equal(0x0, gimbal_calibrate(&gc, 100));

  // maximum
  assert_int_equal(GIMBAL_CALIBRATE_MAX, gimbal_calibrate(&gc, 3900));

  // half way up
  assert_int_equal(0x300, gimbal_calibrate(&gc, (2000 + 3900) / 2));

  // half way down
  assert_int_equal(0x100, gimbal_calibrate(&gc, (100 + 2000) / 2));
}

void test_find_adc_center(void) {
  uint16_t samples[16] = {
    2000, 2023, 1998, 1990,
    2006, 2000, 1989, 1950,
    2025, 2020, 2014, 1956,
    1945, 2032, 2010, 2004,
  };

  assert_int_equal(
      GIMBAL_CALIBRATE_SAMPLES, sizeof(samples) / sizeof(samples[0]));

  struct GimbalCalibrate gc;
  gimbal_cal_init(&gc, 100, 3900);

  for (int i=0; i<GIMBAL_CALIBRATE_SAMPLES; ++i) {
    const uint16_t val = gimbal_calibrate(&gc, samples[i]);
    assert_int_equal(GIMBAL_CALIBRATE_CENTER, val);
    assert_int_equal(gc.num_samples, i+1);
    if (i < GIMBAL_CALIBRATE_SAMPLES - 1) {
      // for these, they passed value is temporarily the new center
      assert_int_equal(gc.adc_center_sample[i], samples[i]);
    } else {
      // at this point, the median calculation is used
      assert_int_equal(2004, samples[i]);
    }
  }

  // doing another check should not change the center
  assert_int_equal(0x285, gimbal_calibrate(&gc, 2500));
  assert_int_equal(gc.adc_center, 2004);
  assert_int_equal(gc.num_samples, GIMBAL_CALIBRATE_SAMPLES);
}

void test_dir_init(void) {
  struct GimbalDirection gd;
  struct GimbalDirection expected_gd;
  memset(&expected_gd, 0, sizeof(struct GimbalDirection));
  expected_gd.deadzone = 23;
  expected_gd.pullback = 123;
  gimbal_dir_init(&gd, 23, 123);
  assert_int_equal(
      0, memcmp(&expected_gd, &gd, sizeof(struct GimbalDirection)));
}

void test_state_transistions(void) {
  struct GimbalDirection gd;
  const uint16_t deadzone = 5;
  const uint16_t pullback = 20;
  gimbal_dir_init(&gd, deadzone, pullback);

  assert_int_equal(CENTERED, gd.state);

  // centered
  assert_int_equal(0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER));
  assert_int_equal(CENTERED, gd.state);
  assert_int_equal(
      0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + deadzone));
  assert_int_equal(CENTERED, gd.state);
  assert_int_equal(
      0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - deadzone));
  assert_int_equal(CENTERED, gd.state);

  // forward movements
  assert_int_equal(
      1, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + deadzone + 1));
  assert_int_equal(TRAVEL_UP_FROM_CENTER, gd.state);
  assert_int_equal(0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + 30));
  assert_int_equal(TRAVEL_UP_FROM_CENTER, gd.state);
  assert_int_equal(0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + 50));
  assert_int_equal(TRAVEL_UP_FROM_CENTER, gd.state);
  assert_int_equal(0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + 40));
  assert_int_equal(TRAVEL_UP_FROM_CENTER, gd.state);
  assert_int_equal(0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + 50));
  assert_int_equal(TRAVEL_UP_FROM_CENTER, gd.state);

  // pullback, then forward to get another blip
  assert_int_equal(
      0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + 50 - pullback - 1));
  assert_int_equal(TRAVEL_DOWN_FROM_ANCHOR, gd.state);
  assert_int_equal(1, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + 50));
  assert_int_equal(TRAVEL_UP_FROM_CENTER, gd.state);

  // another cycle
  assert_int_equal(
      0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + 50 - pullback - 1));
  assert_int_equal(TRAVEL_DOWN_FROM_ANCHOR, gd.state);
  assert_int_equal(1, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + 50));
  assert_int_equal(TRAVEL_UP_FROM_CENTER, gd.state);

  // a very fast pull to negative direction
  assert_int_equal(
      -1, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - deadzone - 1));
  assert_int_equal(TRAVEL_DOWN_FROM_CENTER, gd.state);

  // a very fast pull back to positive direction
  assert_int_equal(
      1, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + deadzone + 1));
  assert_int_equal(TRAVEL_UP_FROM_CENTER, gd.state);

  // pull back to center
  assert_int_equal(
      0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER + deadzone - 1));
  assert_int_equal(CENTERED, gd.state);

  // reverse movements
  assert_int_equal(
      -1, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - deadzone - 1));
  assert_int_equal(TRAVEL_DOWN_FROM_CENTER, gd.state);
  assert_int_equal(0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - 30));
  assert_int_equal(TRAVEL_DOWN_FROM_CENTER, gd.state);
  assert_int_equal(0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - 50));
  assert_int_equal(TRAVEL_DOWN_FROM_CENTER, gd.state);
  assert_int_equal(0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - 40));
  assert_int_equal(TRAVEL_DOWN_FROM_CENTER, gd.state);
  assert_int_equal(0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - 50));
  assert_int_equal(TRAVEL_DOWN_FROM_CENTER, gd.state);

  // pull forward, then back to get another blip
  assert_int_equal(
      0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - 50 + pullback + 1));
  assert_int_equal(TRAVEL_UP_FROM_ANCHOR, gd.state);
  assert_int_equal(-1, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - 50));
  assert_int_equal(TRAVEL_DOWN_FROM_CENTER, gd.state);

  // another cycle
  assert_int_equal(
      0, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - 50 + pullback + 1));
  assert_int_equal(TRAVEL_UP_FROM_ANCHOR, gd.state);
  assert_int_equal(-1, gimbal_direction(&gd, GIMBAL_CALIBRATE_CENTER - 50));
  assert_int_equal(TRAVEL_DOWN_FROM_CENTER, gd.state);
}

int main(void) {
  test(test_cal_init);
  test(test_convert_after_calibration);
  test(test_find_adc_center);
  test(test_dir_init);
  test(test_state_transistions);
  return 0;
}
