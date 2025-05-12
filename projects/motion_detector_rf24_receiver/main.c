// NRF2401 test receiver

#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <rf24/rf24.h>

#define TIMEOUT_MS 5000
#define MOTION_DETECT_COOLOFF_MS 5000
#define GREEN_LED_PORTD 6
#define RED_LED_PORTD 7

// led states
#define LED_OFF 0
#define LED_GREEN (1 << GREEN_LED_PORTD)
#define LED_RED (1 << RED_LED_PORTD)
#define LED_YELLOW (1 << GREEN_LED_PORTD | 1 << RED_LED_PORTD)

// general states
#define NO_SIGNAL 0
#define NO_MOTION_DETECTED 1
#define MOTION_DETECTED 2
#define MOTION_WAS_DETECTED 3

RF24 radio;
uint32_t uptime_ms;  // estimated
uint32_t motion_detect_expire_ms;
uint32_t timeout_ms;
uint8_t state;

// Radio address.  Transmitter and receiver need to match
const uint8_t address[6] = "M0T10N";

static bool_t init_radio(void) {
  rf24_init(&radio, 9, 10); // CE, CSN
  if (!rf24_begin(&radio)) {
    return FALSE;
  }
  rf24_set_payload_size(&radio, 1);
  rf24_set_data_rate(&radio, RF24_250KBPS);
  rf24_set_pa_level(&radio, RF24_PA_HIGH);
  rf24_open_reading_pipe(&radio, 0, address);
  rf24_start_listening(&radio);
  return TRUE;
}


static void no_motion_detected() {
  state = NO_MOTION_DETECTED;
  PORTD &= ~LED_RED;
  PORTD |= LED_GREEN;
}

static void update_state(bool_t motion_detected) {
  if (motion_detected) {
    state = MOTION_DETECTED;
    motion_detect_expire_ms = uptime_ms + MOTION_DETECT_COOLOFF_MS;
    return;
  }

  // Remember that this is the state we *were* in
  switch (state) {
    case NO_SIGNAL:
      no_motion_detected();
      break;
    case NO_MOTION_DETECTED:
      // do nothing
      break;
    case MOTION_DETECTED:
      state = MOTION_WAS_DETECTED;
      break;
    case MOTION_WAS_DETECTED:
      if (uptime_ms >= motion_detect_expire_ms) {
        no_motion_detected();
      }
      break;
  }
}


static void flash_leds_if_needed() {
  static uint8_t frame = 0;

  ++frame;
  if ((frame >> 1) & 0x01) {
    switch (state) {
      case MOTION_DETECTED:
      case MOTION_WAS_DETECTED:
        PORTD &= ~LED_GREEN;
        PORTD |= LED_RED;
        break;
      case NO_SIGNAL:
        PORTD |= LED_YELLOW;
        break;
      default:
        // do nothing
        break;
    }
  } else {
    switch (state) {
      case MOTION_DETECTED:
      case NO_SIGNAL:
        PORTD &= ~LED_YELLOW;  // OFF
        break;
      case MOTION_WAS_DETECTED:
        PORTD &= ~LED_RED;
        PORTD |= LED_GREEN;
        break;
      default:
        // do nothing
        break;
    }
  }
}


static void loop() {
  char msg;
  state = NO_SIGNAL;  // a safe default 
  while (TRUE) {
    while (rf24_available(&radio)) {
      rf24_read(&radio, &msg, 1);
      update_state(msg == 'T');
      timeout_ms = uptime_ms + TIMEOUT_MS;
    }

    if (uptime_ms > timeout_ms) {
      state = NO_SIGNAL;
    }

    flash_leds_if_needed();

    lowpower_powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
    uptime_ms += 120;
  }
}


int main(void) {
  DDRD |= (1 << GREEN_LED_PORTD) | (1 << RED_LED_PORTD); // set as outputs
  // start in error state
  PORTD |= LED_RED;
  while (!init_radio()) {
    // flash the light red
    lowpower_powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    PORTD &= ~LED_RED;
    lowpower_powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    PORTD |= LED_RED;
  }
  loop();
}

