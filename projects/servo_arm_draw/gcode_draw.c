#include "gcode_draw.h"

#include <error_codes.h>

#include "rotary_encoder.h"
#include "servo_common.h"
#include "status.h"

#include <pstr/pstr.h>
#include <servo/servo_atmega.h>
#include <util/delay.h>
#include <math.h>

#define SERVO_DISTANCE_MM 60.0
#define SERVO_ARM_LENGTH_MM 50.0
#define SCISSOR_ARM_LENGTH_MM 80.0
#define PEN_ARM_LENGTH_MM 13.0
#define RIGHT_ARM_LENGTH_MM (SCISSOR_ARM_LENGTH_MM + PEN_ARM_LENGTH_MM)

#define RIGHT_MIN_ANGLE_RAD 0.174532925199 // 10 degrees
#define RIGHT_MAX_ANGLE_RAD 2.09439510239 // 120 degrees

#define LEFT_MIN_ANGLE_RAD 1.0471975512 // 60 degrees
#define LEFT_MAX_ANGLE_RAD 2.96705972839 // 170 degrees

#define ANGLE_RANGE_RAD 1.91986217719  // 120-10 == 170-60 == 110 degrees 

static double g_previous_x = HOME_X;
static double g_previous_y = HOME_Y;
static bool_t g_outside_draw_area = TRUE;

static void move_pen_down(
    const struct ServoCalibration* servo_calibration,
    bool_t pen_down) {
    uint16_t target_us = servo_calibration->pen_down;

    if (!pen_down) {
      // Lots of math, here is an example calculation
      // Say that pen_down is 1500 us and pen_up is at 1550 us
      // This is assumed to be 2mm at 116mm out (verify at gcode_draw.h)
      //
      // Now the pen is 60mm out.  This should be a steeper angle
      // to maintain 2mm.
      //
      // angle: atan 2/60 -> 0.0333
      // target_us = 1500 + (1550 - 1500) * 0.0333 / 0.0172
      //           = 1500 + 50 * 1.933
      //           = 1596

      // we need to calculate the relative angle
      const double dist =
        g_previous_y > MIN_Y_FOR_ANGLE ? g_previous_y : MIN_Y_FOR_ANGLE;

      const double angle = atan2(PEN_UP_MM, dist);  // assume a target height of 1mm
      target_us = (uint16_t)(
        servo_calibration->pen_down +
        (servo_calibration->pen_up - servo_calibration->pen_down) *
        angle / HOME_Y_ANGLE);
    }

    while (!servo_throttled_seek(PEN_SERVO_SLOT, target_us)) {
        _delay_ms(15);
    }
}

static bool_t delay_with_abort(uint16_t ms) {
  for (uint16_t i=0; i < ms; ++i) {
    if (rotary_button_pressed()) {
      status_msg("Draw aborted");
      return TRUE;
    }
    _delay_ms(1);
  }
  return FALSE;
}

// Calculates the right servo angle, in radians
static bool_t calc_right_angle(double* right_angle) {
	// distance between servo gear and final point
	const double d_squared = square(SERVO_DISTANCE_MM / 2 - g_previous_x) + square(g_previous_y);

  const double outer_angle = (
		(square(SERVO_ARM_LENGTH_MM) + d_squared - square(RIGHT_ARM_LENGTH_MM)) /
		(2 * SERVO_ARM_LENGTH_MM * sqrt(d_squared)));

  if (outer_angle >= 1.0) {
    // this point is unreachable
    return FALSE;
	}

	*right_angle = M_PI - (
    atan2(g_previous_y, SERVO_DISTANCE_MM / 2 - g_previous_x) +
    acos(outer_angle));
  return TRUE;
}

static bool_t calc_left_angle(double x, double y, double* left_angle) {
	// distance between servo gear and final point
	const double d_squared = square(SERVO_DISTANCE_MM / 2 + x) + square(y);

  const double outer_angle = (
		(square(SERVO_ARM_LENGTH_MM) + d_squared - square(SCISSOR_ARM_LENGTH_MM)) /
		(2 * SERVO_ARM_LENGTH_MM * sqrt(d_squared)));

  if (outer_angle >= 1.0) {
    // this point is unreachable
    return FALSE;
	}

  *left_angle = atan2(y, SERVO_DISTANCE_MM / 2 + x) + acos(outer_angle);
  return TRUE;
}

static bool_t left_angle_to_us(
  const struct ServoCalibration* servo_calibration,
  double angle,
  uint16_t* left_us) {

  if (angle <= LEFT_MIN_ANGLE_RAD || angle >= LEFT_MAX_ANGLE_RAD) {
    return FALSE;
  }

  // for the left servo
  // INNER_MAX_ANGLE_RAD => servo_calibration.left_low
  // OUTER_MAX_ANGLE_RAD => servo_calibration.left_high
  *left_us =
    servo_calibration->left_low +
      (uint16_t)(
        (angle - LEFT_MIN_ANGLE_RAD) *
         (servo_calibration->left_high - servo_calibration->left_low) /
          ANGLE_RANGE_RAD);

  return TRUE;
}

static bool_t right_angle_to_us(
  const struct ServoCalibration* servo_calibration,
  double angle,
  uint16_t* right_us) {

  if (angle <= RIGHT_MIN_ANGLE_RAD || angle >= RIGHT_MAX_ANGLE_RAD) {
    return FALSE;
  }

  *right_us =
    servo_calibration->right_low +
      (uint16_t)(
        (angle - RIGHT_MIN_ANGLE_RAD) *
         (servo_calibration->right_high - servo_calibration->right_low) /
          ANGLE_RANGE_RAD);

  return TRUE;
}

// translate x,y coordinates into servo us for left and right.
// returns FALSE, if this is not possible.
static bool_t calc_servo_us(
  const struct ServoCalibration* servo_calibration,
  uint16_t* left_us,
  uint16_t* right_us) {
  double right_servo_angle;

	if (!calc_right_angle(&right_servo_angle)) {
    return FALSE;
  }

	const double right_arm_x =
      SERVO_DISTANCE_MM / 2 + SERVO_ARM_LENGTH_MM * cos(right_servo_angle);
	const double right_arm_y = SERVO_ARM_LENGTH_MM * sin(right_servo_angle);

	const double delta_x = g_previous_x - right_arm_x;
	const double join_x =
      right_arm_x + delta_x * (SCISSOR_ARM_LENGTH_MM / RIGHT_ARM_LENGTH_MM); 
	
	const double delta_y = g_previous_y - right_arm_y;
	const double join_y =
      right_arm_y + delta_y * (SCISSOR_ARM_LENGTH_MM / RIGHT_ARM_LENGTH_MM); 

  double left_servo_angle;
  if (!calc_left_angle(join_x, join_y, &left_servo_angle)) {
    return FALSE;
  }

  if (!left_angle_to_us(servo_calibration, left_servo_angle, left_us)) {
    return FALSE;
  }

  if (!right_angle_to_us(servo_calibration, right_servo_angle, right_us)) {
    return FALSE;
  }

  return TRUE;
}

static bool_t plot_to(
    const struct ServoCalibration* servo_calibration,
    bool_t pen_is_down,
    bool_t move_pen_first) {

    // See if we can reach the target
    uint16_t left_servo_us;
    uint16_t right_servo_us;
    if (!calc_servo_us(
        servo_calibration,
        &left_servo_us,
        &right_servo_us)) {
      // Can not reach this position
      move_pen_down(servo_calibration, FALSE);
      g_outside_draw_area = TRUE;
      return TRUE;
    }

    if (g_outside_draw_area) {
      // we are in bounds now, but this movement should be a slow seek
      g_outside_draw_area = FALSE;
      bool_t target_found = FALSE;
      while (!target_found) {
        target_found =
            servo_throttled_seek(LEFT_ARM_SERVO_SLOT, left_servo_us) &&
            servo_throttled_seek(RIGHT_ARM_SERVO_SLOT, left_servo_us);
        if (delay_with_abort(20)) {
          // aborted
          return FALSE;
        }
      }
      return TRUE;
    }

    if (move_pen_first) {
      move_pen_down(servo_calibration, pen_is_down);
    }
    servo_set_period_us(LEFT_ARM_SERVO_SLOT, left_servo_us);
    servo_set_period_us(RIGHT_ARM_SERVO_SLOT, right_servo_us);
    if (!move_pen_first) {
      move_pen_down(servo_calibration, pen_is_down);
    }
    return TRUE;
}

bool_t gcode_draw(
    const struct GCodeParseData* data,
    const struct ServoCalibration* servo_calibration) {
  
  // We need to trace from the current position to the target based on the
  // feed rate.

  double delta_x = data->x - g_previous_x;
  double delta_y = data->y - g_previous_y;
  double line_length = sqrt(square(delta_x) + square(delta_y));
  if (line_length < 0.05) {
    // Too short to bother
    return TRUE;
  }
  delta_x = delta_x * data->feed_rate / line_length / 50;  // / 50 because we delay for 20ms
  delta_y = delta_y * data->feed_rate / line_length / 50;

  while (TRUE) {
    // move pen first (only relevant when the pen is up)
    // if delta_y < 0 then the pen is moving toward the base
    // in this case, we want to move the pen beofre x,y
    // The opposite is true when delta_y > 0
    if (!plot_to(servo_calibration, data->pen_is_down, delta_y < 0)) {
      return FALSE;
    }
    g_previous_x += delta_x;
    g_previous_y += delta_y;
    if (!g_outside_draw_area) {
      if (delay_with_abort(20 - 1)) {  // 1 ms for overhead
        return FALSE;  // User aborted
      }
    }

    // Check completion conditions
    if (delta_x > 0 && g_previous_x > data->x) {
      break;
    }
    if (delta_x < 0 && g_previous_x < data->x) {
      break;
    }
    if (delta_y > 0 && g_previous_y > data->y) {
      break;
    }
    if (delta_y < 0 && g_previous_y < data->y) {
      break;
    }
  }

  g_previous_x = data->x;
  g_previous_y = data->y;
  if (!plot_to(servo_calibration, data->pen_is_down, delta_y < 0)) {
    return FALSE;
  }
  
  return TRUE;
}