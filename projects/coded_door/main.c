// Implements a "coded door" toy.
//
//  The setup is a servo that powers a door, three buttons, and a buzzer.
//  You need to push the buttons in the correct order to open the door.
//  If you fail to do so, the buzzer sounds instead.
//
//  If the door is open, any button will close it.
//
//         +--------+
// N/C    -|        |- VDD
// BUZZER -| Tiny85 |- BLACK Button Input (Switch connected to ground)
// SERVO  -|        |- BLUE Button Input (switch connected to ground)
// VSS    -|        |- YELLOW Button Input (switch connected to ground)
//         +--------+

#include <avr/io.h>
#include <servo/servo_attiny.h>
#include <util/delay.h>
#include <misc/debounce.h>

#define SERVO_PIN PB4
#define BLACK_PIN PB2
#define YELLOW_PIN PB0
#define BLUE_PIN PB1
#define BUZZER_PIN PB3
// This is used when the user pressed multiple buttons at the same time
#define INVALID_PIN 100

#define CLOSED_PERIOD 2175
#define OPEN_PERIOD 1620
#define CODE_ERROR_MS 500
#define CODE_ENTER_MS 20

#define DEBOUNCE_TIME_MS 10
#define DOOR_TRANSITION_MS 750
#define SERVO_UPDATE_MS 10

const uint8_t code[] = {
  BLACK_PIN,
  BLUE_PIN,
  YELLOW_PIN,
  BLUE_PIN,
};

// What step of the code we are on
uint8_t code_index;
// If the code is correct or not
bool_t is_matching;
// If true, we are looking for a rising edge
bool_t look_for_rising_edge;
// If true, the door is open
bool_t door_is_open;
// Current system clock, updated by poll to avoid
// setting up additional interrupts.  Extreme accuracy is not
// needed.
uint32_t time_ms;

struct Debounce black_db;
struct Debounce yellow_db;
struct Debounce blue_db;

// A delay function which also adds to time_ms
static void my_delay_ms(uint16_t delay) {
  for (uint16_t i=0; i < delay; ++i) {
    _delay_ms(1);
  }
  time_ms += delay;
}

// Opens or shuts the door.  Uses a loop to slow down the open a bit
void set_door(bool_t open) {
  door_is_open = open;

  // Decide what the final PWM period should be.
  const uint16_t target_period = door_is_open ? OPEN_PERIOD : CLOSED_PERIOD;
  // Get the current actual period, which may be unknown
  uint16_t period_us = servo_get_period_us();
  if (period_us == 0) {
    // Not intialized so the current state is not known
    servo_set(SERVO_PIN, target_period);
    return;
  }

  // Calculate the number of steps to move the servo each loop cycle
  // 
  // Not fully accurate due to integer roundoff but should be close enough
  //
  // Example: 10 * (2000 - 1000) / 2500
  //        = 10 * 1000 / 2500
  //        = 4 steps per cycle
  //
  // For a 2.5 second transition, we would need (2500 / 10) = 250 cycles.
  // 250 * 4 = 1000 which is the needed travel distance
#if OPEN_PERIOD > CLOSED_PERIOD
  const uint8_t steps_per_cycle = SERVO_UPDATE_MS * (OPEN_PERIOD - CLOSED_PERIOD) / DOOR_TRANSITION_MS;
#else
  const uint8_t steps_per_cycle = SERVO_UPDATE_MS * (CLOSED_PERIOD - OPEN_PERIOD) / DOOR_TRANSITION_MS;
#endif

  // Now the period_us is stepped up or down until it hits the final target
  while (period_us != target_period) {
    if (period_us > target_period) {
      if (period_us > (target_period + steps_per_cycle)) {
        period_us -= steps_per_cycle;
      } else {
        period_us = target_period;
      }
    } else {
      if (period_us < (target_period - steps_per_cycle)) {
        period_us += steps_per_cycle;
      } else {
        period_us = target_period;
      }
    }
    servo_set(SERVO_PIN, period_us);
    my_delay_ms(SERVO_UPDATE_MS);
  }
}

// Activate the buzzer for the specified interval
void buzzer(uint16_t time_on_ms) {
  PORTB |= (1 << BUZZER_PIN);
  my_delay_ms(time_on_ms);
  PORTB &= ~(1 << BUZZER_PIN);
}

// Resets to look for a new code
void reset_code(void) {
  is_matching = TRUE;
  code_index = 0;
  look_for_rising_edge = TRUE;
  // On servo moves, there is a long time between samples
  // which the debounce code does not like.
  debounce_init(&black_db, DEBOUNCE_TIME_MS);
  debounce_init(&yellow_db, DEBOUNCE_TIME_MS);
  debounce_init(&blue_db, DEBOUNCE_TIME_MS);
}

// Checks the given code to see if it's still valid
//
// This function assumes that the door is closed.
void check_code(uint8_t pin_changed, bool_t pressed) {
  if (look_for_rising_edge != pressed) {
    // Multiple buttons we held down at the same time
    // Thus we doubled up on the pressed state
    is_matching = FALSE;
  }

  if (pin_changed != code[code_index]) {
    // The button that was pressed or released is not
    // the one needed for the code
    is_matching = FALSE;
  }

  if (pressed) {
    look_for_rising_edge = FALSE;
    buzzer(CODE_ENTER_MS);  // A short "got your button press" beep.
  } else {
    ++code_index;
    if (code_index == (sizeof(code) / sizeof(code[0]))) {
      if (is_matching) {
        set_door(TRUE);
      } else {
        // Signal that the code is wrong
        buzzer(CODE_ERROR_MS);
      }
      reset_code();
    }
    look_for_rising_edge = TRUE;
  }
}

// Checks the given button to see if it's pressed.  Returns true if
// the button was edge triggered.
//  db: The debounce structure
//  pin: The pin number, eg BLACK_PIN
//  pin_changed: Updated to pin if the button was edge triggered
//  pressed: Updated to db->pressed if the button was edge triggered
bool_t check_a_button(
    struct Debounce* db,
    uint8_t pin,
    uint8_t* pin_changed,
    uint8_t* pressed) {
  if (debounce_sample(db, time_ms, !(PINB & (1 << pin))) && db->val) {
    *pin_changed = pin;
    *pressed = 1;
    return TRUE;
  }
  return FALSE;
}

// Checks all buttons and returns the one that was pressed
// If multiple button changes occurred, sets pin_changed
// to INVALID_PIN
//
// Returns TRUE if something changed.
//
//   pin_changed: Set to the pin that was changed only if triggered
//   pressed: Set the the pin state only if triggered
bool_t check_buttons(uint8_t* pin_changed, uint8_t* pressed) {
  uint8_t change_count = 0;
  
  if (check_a_button(&black_db, BLACK_PIN, pin_changed, pressed)) {
    ++change_count;
  }

  if (check_a_button(&yellow_db, YELLOW_PIN, pin_changed, pressed)) {
    ++change_count;
  }

  if (check_a_button(&blue_db, BLUE_PIN, pin_changed, pressed)) {
    ++change_count;
  }

  if (change_count > 1) {
    *pin_changed = INVALID_PIN;
  }

  return (change_count > 0);
}

int main(void) {
  set_door(FALSE);
  DDRB |= 1 << BUZZER_PIN;  // Buzzer is an output
  // Enable internal pullups
  PORTB |= (1 << BLACK_PIN) | (1 << YELLOW_PIN) | (1 << BLUE_PIN);
  reset_code();

  for (;; ++time_ms) {
    uint8_t pin_changed = 0;
    uint8_t pressed = 0;

    if (check_buttons(&pin_changed, &pressed)) {
      if (door_is_open) {
        if (pressed) {
          // Close the door
          set_door(FALSE);
          reset_code();
        }
      } else {
        check_code(pin_changed, pressed);
      }
    }

    // Estimated to make the overall time about 1 ms
    _delay_us(750);
  }
}
