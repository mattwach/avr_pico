#include "calibrate_servos.h"

#include <util/delay.h>

#include <pstr/pstr.h>
#include <servo/servo_atmega.h>

#include "rotary_encoder.h"
#include "servo_common.h"

static struct ServoCalibration* g_servo_cal = 0;
static struct Text* g_text;  // needed to draw chars
static bool_t g_is_saved;  // If TRUE, there is no need to store to EEPROM
static bool_t g_button_toggle;  // used to detect rotary putton presses

// State:
#define STATE_LEFT_LOW 0
#define STATE_LEFT_MIDDLE 1
#define STATE_LEFT_HIGH 2
#define STATE_RIGHT_LOW 3
#define STATE_RIGHT_MIDDLE 4
#define STATE_RIGHT_HIGH 5
#define STATE_PEN_UP 6
#define STATE_PEN_DOWN 7
#define STATE_SAVE_BUTTON 8

static uint8_t g_state;

// UI columns
#define LEFT_COLUMN 2
#define RIGHT_COLUMN 40
#define PEN_COLUMN 78

#define LOW_MIN 800
#define LOW_MAX 1400

#define HIGH_MIN 1600
#define HIGH_MAX 2200

#define PEN_MIN 1000
#define PEN_MAX 2000

// Draws one state field.  Puts brackets [] around the value if
// display_state == g_state
static void draw_value(uint8_t display_state) {
  // retrieve the needed value
  uint16_t val = 1500;
  switch (display_state) {
    case STATE_LEFT_LOW:
      val = g_servo_cal->left_low;
      break;
    case STATE_LEFT_HIGH:
      val = g_servo_cal->left_high;
      break;
    case STATE_RIGHT_LOW:
      val = g_servo_cal->right_low;
      break;
    case STATE_RIGHT_HIGH:
      val = g_servo_cal->right_high;
      break;
    case STATE_PEN_UP:
      val = g_servo_cal->pen_up;
      break;
    case STATE_PEN_DOWN:
      val = g_servo_cal->pen_down;
      break;
  }

  // UI positioning logic.
  if (display_state <= STATE_LEFT_HIGH) {
    g_text->column = LEFT_COLUMN;
    g_text->row = 3 + display_state;
  } else if (display_state <= STATE_RIGHT_HIGH) {
    g_text->column = RIGHT_COLUMN;
    g_text->row = display_state;
  } else if (display_state <= STATE_PEN_DOWN) {
    g_text->column = PEN_COLUMN;
    g_text->row = display_state - 3;
  } else {
    g_text->row = 7;
    g_text->column = 37;
  }

  text_char(g_text, display_state == g_state ? '[' : ' ');
  if (display_state <= STATE_PEN_DOWN) {
    if (val < 1000) {
      text_char(g_text, ' '); // pad to 4 chars, we assume its 3 chars.
    }
    text_pstr(g_text, u16_to_ps(val));
  } else {
    text_str(g_text,
      g_is_saved ? "  Saved  " : "NOT SAVED");
  }
  text_char(g_text, display_state == g_state ? ']' : ' ');
}

// Draws a line of --- across the display
static void hline(uint8_t row) {
  g_text->row = row;
  g_text->column = 0;
  for (uint8_t i=0; i < 21; ++i) {
    text_char(g_text, '-');
  }
}

// called when the rotary button is pressed.
static void button_pressed() {
  // We don't bother seleting the "Saved" UI element when everythings saved.
  const uint8_t max_state = g_is_saved ? STATE_PEN_DOWN : STATE_SAVE_BUTTON;

  // need to remember the old one for later
  const uint8_t old_state = g_state;

  ++g_state;
  if (g_state > max_state) {
    g_state = STATE_LEFT_LOW;
  }

  uint8_t servo_slot = LEFT_ARM_SERVO_SLOT;
  switch (g_state) {
    case STATE_LEFT_LOW:
      rotary_set(LOW_MIN, LOW_MAX, g_servo_cal->left_low);
      break;
    case STATE_LEFT_MIDDLE:
      break;
    case STATE_LEFT_HIGH:
      rotary_set(HIGH_MIN, HIGH_MAX, g_servo_cal->left_high);
      break;
    case STATE_RIGHT_LOW:
      servo_slot = RIGHT_ARM_SERVO_SLOT;
      rotary_set(LOW_MIN, LOW_MAX, g_servo_cal->right_low);
      break;
    case STATE_RIGHT_MIDDLE:
      servo_slot = RIGHT_ARM_SERVO_SLOT;
      break;
    case STATE_RIGHT_HIGH:
      servo_slot = RIGHT_ARM_SERVO_SLOT;
      rotary_set(HIGH_MIN, HIGH_MAX, g_servo_cal->right_high);
      break;
    case STATE_PEN_UP:
      servo_slot = PEN_SERVO_SLOT;
      rotary_set(g_servo_cal->pen_down + 1, PEN_MAX, g_servo_cal->pen_up);
      break;
    case STATE_PEN_DOWN:
      servo_slot = PEN_SERVO_SLOT;
      rotary_set(PEN_MIN, g_servo_cal->pen_up - 1, g_servo_cal->pen_down);
      break;
    case STATE_SAVE_BUTTON:
      rotary_set(1, 3, 2);
      break;
    default:
      // For everything else, the rotary keeps it's old state and it's output
      // is ignored.
      break;
  }

  draw_value(old_state);  // deselect the old one
  draw_value(g_state);    // select the new one
}

// Saves state to EEPROM
static void save_state(void) {
  if (g_is_saved) {
    return;  // already saved
  }
  servo_save_state();
  g_is_saved = TRUE;
  draw_value(STATE_SAVE_BUTTON);  // Update UI to say "Saved"
}

// Checks for button presses or rotary changes
static void check_input(void) {
  if (rotary_button_toggle() != g_button_toggle) {
    g_button_toggle = !g_button_toggle;
    button_pressed();
    return;
  }

  uint8_t servo_slot = LEFT_ARM_SERVO_SLOT;
  uint16_t* target = 0;
  switch (g_state) {
    case STATE_LEFT_LOW:
      target = &(g_servo_cal->left_low);
      break;
    case STATE_LEFT_HIGH:
      target = &(g_servo_cal->left_high);
      break;
    case STATE_RIGHT_LOW:
      servo_slot = RIGHT_ARM_SERVO_SLOT;
      target = &(g_servo_cal->right_low);
      break;
    case STATE_RIGHT_HIGH:
      servo_slot = RIGHT_ARM_SERVO_SLOT;
      target = &(g_servo_cal->right_high);
      break;
    case STATE_PEN_UP:
      servo_slot = PEN_SERVO_SLOT;
      target = &(g_servo_cal->pen_up);
      break;
    case STATE_PEN_DOWN:
      servo_slot = PEN_SERVO_SLOT;
      target = &(g_servo_cal->pen_down);
      break;
    case STATE_SAVE_BUTTON:
      // "2" is the dummy init value set in button_pressed()
      // Here we see if it changed from 2 to something else.
      if (rotary_value() != 2) {
        save_state();
      }
      return; 
    default:
      // rotary not used
      return;
  }

  const uint16_t val = rotary_value();
  if (*target != val) {
    if (g_is_saved) {
      // Need to update the UI to allow for a save.
      g_is_saved = FALSE;
      draw_value(STATE_SAVE_BUTTON);
    }
    *target = val;
    draw_value(g_state); // Update UI with new value
  }
}

static bool_t seek_servos() {
  bool_t all_at_target = TRUE;
  // set non-active servos
  if (g_state != STATE_LEFT_LOW && g_state != STATE_LEFT_HIGH) {
    all_at_target &= servo_throttled_seek(LEFT_ARM_SERVO_SLOT, 1500);
  }

  if (g_state != STATE_RIGHT_LOW && g_state != STATE_RIGHT_HIGH) {
    all_at_target &= servo_throttled_seek(RIGHT_ARM_SERVO_SLOT, 1500);
  }

  if (g_state != STATE_PEN_DOWN) {
    all_at_target &= servo_throttled_seek(PEN_SERVO_SLOT, g_servo_cal->pen_up);
  }

  // set active servo
  switch (g_state) {
    case STATE_LEFT_LOW:
      all_at_target &= servo_throttled_seek(
        LEFT_ARM_SERVO_SLOT, g_servo_cal->left_low);
      break;
    case STATE_LEFT_HIGH:
      all_at_target &= servo_throttled_seek(
        LEFT_ARM_SERVO_SLOT, g_servo_cal->left_high);
      break;
    case STATE_RIGHT_LOW:
      all_at_target &= servo_throttled_seek(
        RIGHT_ARM_SERVO_SLOT, g_servo_cal->right_low);
      break;
    case STATE_RIGHT_HIGH:
      all_at_target &= servo_throttled_seek(
        RIGHT_ARM_SERVO_SLOT, g_servo_cal->right_high);
      break;
    case STATE_PEN_DOWN:
      all_at_target &= servo_throttled_seek(
        PEN_SERVO_SLOT, g_servo_cal->pen_down);
      break;
    default:
      // all at defauilts
      break;
  }

  return all_at_target;
}

static void init_servos_if_needed(void) {
  if (!g_servo_cal) {
    g_servo_cal = servo_common_init();
  } else {
    servo_set_count(3);
  }
}

void calibrate_servos_pen_home(bool_t pen_down) {
  init_servos_if_needed();
  g_state = pen_down ? STATE_PEN_DOWN : STATE_PEN_UP;
  while (!seek_servos()) {
    _delay_ms(20);
  }
}

void calibrate_servos_ui(struct Text* text) {
  g_text = text;

  // Everything below only needs to be drawn once for the lifetime of the UI.
  g_text->row = 0;
  g_text->column = 13;
  text_str(g_text, "Servo Calibration");

  hline(1);

  g_text->row = 2;
  g_text->column = LEFT_COLUMN;
  text_str(g_text, " LEFT");

  g_text->column = RIGHT_COLUMN;
  text_str(g_text, " RIGHT");

  g_text->column = PEN_COLUMN;
  text_str(g_text, " PEN");

  hline(6);

  g_button_toggle = rotary_button_toggle();  // capture current state
  init_servos_if_needed();
  g_is_saved = TRUE;
  g_state = STATE_LEFT_LOW;
  rotary_set(LOW_MIN, LOW_MAX, g_servo_cal->left_low);

  // Draw each value once.  After this, they will only draw when they change.
  for (uint8_t display_state = 0; display_state <= 8; ++display_state) { 
    draw_value(display_state);
  }

  while (1) {
    check_input();
    seek_servos();
    rotary_poll_ms(15);
  }
}

