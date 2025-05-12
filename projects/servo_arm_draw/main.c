#include <avr/interrupt.h>

#include <oledm/oledm.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus6x8.h>
#include <oledm/text.h>

#include "calibrate_servos.h"
#include "file_select.h"
#include "gcode_file.h"
#include "rotary_encoder.h"
#include "status.h"

#define OLEDM_INIT oledm_basic_init

static struct Text text;
static struct OLEDM display;

static void wait_for_press(void) {
  status_start_on(7, "Press button to reset");
  status_end();
  while (!rotary_button_pressed()) {
    rotary_poll_ms(10);
  }
}

int main(void) {
  sei();
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  rotary_init(0, 0, 0);
  text_init(&text, terminus6x8, &display);
  status_init(&text);
  file_select_init();

  oledm_clear(&display, 0x00);
  uint32_t file_size;

  while (TRUE) {
    int8_t selection = file_select(&text, &file_size);
    oledm_clear(&display, 0x00);
    switch (selection) {
      case FILE_SELECT_OPEN_ERROR:
        status_msg("OPEN ERROR");
        wait_for_press();
        break;
      case FILE_SELECT_CALIBRATE_SERVOS:
        calibrate_servos_ui(&text);
        break;
      case FILE_SELECT_PEN_DOWN:
        calibrate_servos_pen_home(TRUE);
        break;
      case FILE_SELECT_PEN_UP:
        calibrate_servos_pen_home(FALSE);
        break;
      case FILE_SELECT_FILE_TEST:
        gcode_file_draw(file_size, TRUE);
        wait_for_press();
        break;
      case FILE_SELECT_FILE_OPEN:
        gcode_file_draw(file_size, FALSE);
        wait_for_press();
        break;
      default:
        status_msg("INTERNAL CASE");
        wait_for_press();
        break;
    }
  }
}
