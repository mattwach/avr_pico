#ifndef GCODE_DRAW_H
#define GCODE_DRAW_H

#include "gcode_parser.h"
#include "servo_common.h"

#include <error_codes.h>

#define HOME_X 0
#define HOME_Y 116

// estimated as we don't calibrate for this
#define PEN_UP_MM 2
// This is calculated as atan 2/116
// We see that HOME_Y is 116 above and asume that the UP position is about 1mm
// above th paper
#define HOME_Y_ANGLE  0.0172413793103
// at y=0, the calculation would be 2/0 which we clearly don't want.
// so we set a min.  At 10, we would have atan 2/10 -> 11.3 degrees
#define MIN_Y_FOR_ANGLE 10

// Draws or moves one gcode step.  Returns false if this failed for some reason
bool_t gcode_draw(
    const struct GCodeParseData* data,
    const struct ServoCalibration* servo_calibration);

#endif