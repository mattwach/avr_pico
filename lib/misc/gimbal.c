#include "gimbal.h"
#include "gimbal_state.h"
#include <string.h>

void gimbal_cal_init(
    struct GimbalCalibrate* gc,
    uint16_t adc_min,
    uint16_t adc_max) {
  memset(gc, 0, sizeof(struct GimbalCalibrate));
  gc->adc_min = adc_min;
  gc->adc_max = adc_max;
}

static void _calibrate(struct GimbalCalibrate* gc, uint16_t adc_value) {
  gc->adc_center_sample[gc->num_samples] = adc_value;
  ++gc->num_samples;
  gc->adc_center = adc_value;
  if (gc->num_samples < GIMBAL_CALIBRATE_SAMPLES) {
    // no enough data for sorting
    return;
  }
  // take the median value as our final one.  Use a simple swap
  // sort since the sata size is small.
  uint8_t sorted = 0;
  while (!sorted) {
    sorted = 1;
    for (int i=0; i<(GIMBAL_CALIBRATE_SAMPLES - 1); ++i) {
      // before I moved these values to a and b, the > operator was giving
      // sporatic results saying things like 1958 > 2000.  Not sure why.
      const uint16_t a = gc->adc_center_sample[i];
      const uint16_t b = gc->adc_center_sample[i+1];
      if (a > b) {
        gc->adc_center_sample[i] = b;
        gc->adc_center_sample[i+1] = a;
        sorted = 0;
      }
    }
  }

  // median value is our chosen center
  gc->adc_center = gc->adc_center_sample[GIMBAL_CALIBRATE_SAMPLES / 2];
}

uint16_t gimbal_calibrate(struct GimbalCalibrate* gc, uint16_t adc_value) {
  if (gc->num_samples < GIMBAL_CALIBRATE_SAMPLES) {
    _calibrate(gc, adc_value); 
  }
  if (adc_value >= gc->adc_max) {
    gc->adc_max = adc_value;
    return GIMBAL_CALIBRATE_MAX;
  }
  if (adc_value <= gc->adc_min) {
    gc->adc_min = adc_value;
    return 0;
  }

  // We consider GIMBAL_CALIBRATE_CENTER to be the middle point

  if (adc_value >= gc->adc_center) {
    // use linear interpolation to map the adc value to the ten bit scale
    const uint32_t offset = adc_value - gc->adc_center;
    const uint32_t span = gc->adc_max - gc->adc_center;
    return GIMBAL_CALIBRATE_CENTER + (offset * GIMBAL_CALIBRATE_CENTER / span);
  } else {
    const uint32_t offset = gc->adc_center - adc_value;
    const uint32_t span = gc->adc_center - gc->adc_min;
    return GIMBAL_CALIBRATE_CENTER - (offset * GIMBAL_CALIBRATE_CENTER / span);
  }
}

static int8_t _gimbal_seldir_centered(
    struct GimbalDirection* gd,
    uint16_t calv) {
  if (calv > (GIMBAL_CALIBRATE_CENTER + gd->deadzone)) {
    gd->anchor = GIMBAL_CALIBRATE_CENTER;
    gd->state = TRAVEL_UP_FROM_CENTER;
    return 1;
  }
  if (calv < (GIMBAL_CALIBRATE_CENTER - gd->deadzone)) {
    gd->anchor = GIMBAL_CALIBRATE_CENTER;
    gd->state = TRAVEL_DOWN_FROM_CENTER;
    return -1;
  }
  return 0;
}

static int8_t _gimbal_seldir_travel_up_from_center(
    struct GimbalDirection* gd,
    uint16_t calv) {
  if (calv < (GIMBAL_CALIBRATE_CENTER + gd->deadzone)) {
    // a sudden move in the reverse direction
    gd->state = CENTERED;
    return _gimbal_seldir_centered(gd, calv);
  } else if (calv > gd->anchor) {
    // still moving up
    gd->anchor = calv;
  } else if (calv < (gd->anchor - gd->pullback)) {
    gd->anchor = calv;
    gd->state = TRAVEL_DOWN_FROM_ANCHOR;
  }
  return 0;
}

static int8_t _gimbal_seldir_travel_down_from_center(
    struct GimbalDirection* gd,
    uint16_t calv) {
  if (calv > (GIMBAL_CALIBRATE_CENTER - gd->deadzone)) {
    // a sudden move in the reverse direction
    gd->state = CENTERED;
    return _gimbal_seldir_centered(gd, calv);
  } else if (calv < gd->anchor) {
    // still moving up
    gd->anchor = calv;
  } else if (calv > (gd->anchor + gd->pullback)) {
    gd->anchor = calv;
    gd->state = TRAVEL_UP_FROM_ANCHOR;
  }
  return 0;
}

static int8_t _gimbal_seldir_travel_down_from_anchor(
    struct GimbalDirection* gd,
    uint16_t calv) {
  if (calv > (gd->anchor + gd->pullback)) {
    // user is signalling an up direction
    gd->anchor = calv;
    gd->state = TRAVEL_UP_FROM_CENTER;
    return 1;
  }
  if (calv < gd->anchor) {
    gd->anchor = calv;
  }
  if (calv < (GIMBAL_CALIBRATE_CENTER + gd->deadzone)) {
    gd->state = CENTERED;
  }
  return 0;
}

static int8_t _gimbal_seldir_travel_up_from_anchor(
    struct GimbalDirection* gd,
    uint16_t calv) {
  if (calv < (gd->anchor - gd->pullback)) {
    // user is signalling a down direction
    gd->anchor = calv;
    gd->state = TRAVEL_DOWN_FROM_CENTER;
    return -1;
  }
  if (calv > gd->anchor) {
    gd->anchor = calv;
  }
  if (calv > (GIMBAL_CALIBRATE_CENTER + gd->deadzone)) {
    gd->state = CENTERED;
  }
  return 0;
}

void gimbal_dir_init(
    struct GimbalDirection* gd,
    uint16_t deadzone,
    uint16_t pullback) {
  memset(gd, 0, sizeof(struct GimbalDirection));
  gd->deadzone = deadzone;
  gd->pullback = pullback;
  //gd->state = CENTERED;  already zero
  //gd->dir.anchor = 0;  already zero
}


int8_t gimbal_direction(struct GimbalDirection *gd, uint16_t calv) {
  switch (gd->state) {
    case CENTERED:
      return _gimbal_seldir_centered(gd, calv);
    case TRAVEL_UP_FROM_CENTER:
      return _gimbal_seldir_travel_up_from_center(gd, calv);
    case TRAVEL_DOWN_FROM_CENTER:
      return _gimbal_seldir_travel_down_from_center(gd, calv);
    case TRAVEL_DOWN_FROM_ANCHOR:
      return _gimbal_seldir_travel_down_from_anchor(gd, calv);
    case TRAVEL_UP_FROM_ANCHOR:
      return _gimbal_seldir_travel_up_from_anchor(gd, calv);
  }

  // It's a bug if this code is reached
  return 0;
}

