#ifndef CALIBRATE_SERVOS_H
#define CALIBRATE_SERVOS_H

#include <oledm/text.h>
#include <error_codes.h>

// Display UI for calibrating servos
void calibrate_servos_ui(struct Text* text);


// Move the pen home and up/down
void calibrate_servos_pen_home(bool_t pen_down);

#endif
