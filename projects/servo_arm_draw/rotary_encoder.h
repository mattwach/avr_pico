#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <error_codes.h>

// Access to rotatry encoder

// Call this function to delay instead of _delay_ms.  It will actively
// poll the rotary encoder.
void rotary_poll_ms(uint16_t ms);

// Instead of calling rotary_poll_ms, you can opt to use interrupts.
// Note that using interrupts and trying to control servos at
// the same time leads to erratic servo behavior.
void rotary_use_interrupts(bool_t use);

// Initialize the rotary encoder, clears button state
void rotary_init(uint16_t min_val, uint16_t max_val, uint16_t val);

// Change the rotary encoder limits and current value.  Does not change button state
void rotary_set(uint16_t min_val, uint16_t max_val, uint16_t val);

// Every time the rotary button is pressed, this value
// toggles from FALSE->TRUE->FALSE...
bool_t rotary_button_toggle(void);

// Returns true if the rotary button is down right now.
// rotary_button_toggle is generally the better option to
// not miss presses, but this can be useful to detect if the
// button is being held down
bool_t rotary_button_pressed(void);

// Returns the current rotary encoder value
uint16_t rotary_value(void);

#endif
