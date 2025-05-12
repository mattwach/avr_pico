#include "gcode_file.h"

#include <util/delay.h>

#include <pstr/pstr.h>

#include "gcode_draw.h"
#include "gcode_parser.h"
#include "rotary_encoder.h"
#include "servo_common.h"
#include "status.h"

static struct GCodeParseData gcode_data;
#define READ_BUFFER_SIZE 128
unsigned int g_read_buf_available;
unsigned int g_read_buf_idx;
static char g_read_buf[READ_BUFFER_SIZE];

#define ERROR_ROW 5
static void error_status() {
  status_start_on(ERROR_ROW, "GCODE: ");
  status_pstr(u16_to_ps(gcode_data.line_number));
  status_str(": (");
  status_pstr(u8_to_ps(gcode_data.error));
  status_str(") ");
  status_end();

  status_start_on(ERROR_ROW + 1, "  ");
  status_str(gcode_error_str(gcode_data.error));
  status_end();
}

static void progress_status(
  bool_t check_mode,
  uint8_t percent_done,
  uint32_t total_bytes_read,
  uint32_t lines_drawn,
  uint32_t moves) {
  status_start(check_mode ? "Check: " : "Draw: ");
  status_pstr(u8_to_ps(percent_done));
  status_char('%');
  status_end();

  status_start_on_pstr(1, u32_to_ps(total_bytes_read));
  status_str(" Bytes");
  status_end();

  status_start_on_pstr(2, u32_to_ps(lines_drawn));
  status_str(" Lines");
  status_end();

  status_start_on_pstr(3, u32_to_ps(moves));
  status_str(" Moves");
  status_end();
}

// Returns true if a byte was read
static bool_t buffered_read(char* c) {
  if ((g_read_buf_idx >= g_read_buf_available) &&  // No buffered data
      (g_read_buf_available == 0 ||  // initial condition
       g_read_buf_available == READ_BUFFER_SIZE)  // more data avail
     ) {
    g_read_buf_idx = 0;
    FRESULT result = pf_read(
      &g_read_buf,
      READ_BUFFER_SIZE,
      &g_read_buf_available);

    if (result != FR_OK) {
      status_fresult("pf_read", result);
      g_read_buf_available = 0;
    }
  }

  if (g_read_buf_idx < g_read_buf_available) {
    *c = g_read_buf[g_read_buf_idx++];
    return TRUE;
  }

  // The end was reached
  return FALSE;
}

static FRESULT process_gcode_file(
  const struct ServoCalibration* servo_calibration,
  uint32_t file_size) {
  const bool_t check_mode = servo_calibration == 0;
  char c;
  FRESULT result = FR_OK;
  uint32_t total_bytes_read = 0;
  uint32_t moves = 0;
  uint32_t lines_drawn = 0;
  uint8_t status_pct_shown = 255;
  gcode_parser_init(&gcode_data);

  result = status_fresult("seek", pf_lseek(0));
  if (result != FR_OK) {
    return result;
  }

  // initialize buffer
  g_read_buf_available = 0;
  g_read_buf_idx = 0;

  while (TRUE) {
    char c;
    if (!buffered_read(&c)) {
      break;
    }

    const bool_t code_ready = gcode_parse(c, &gcode_data);
    if (gcode_data.error) {
      error_status();
      return FR_NOT_READY;
    }

    ++total_bytes_read;
    if (code_ready) {
      if (gcode_data.pen_is_down) {
        ++lines_drawn;
      } else {
        ++moves;
      }
      const uint8_t status_pct = (uint8_t)(total_bytes_read * 100 / file_size);
      if (status_pct != status_pct_shown) {
        progress_status(
          check_mode,
          status_pct,
          total_bytes_read,
          lines_drawn,
          moves);
        status_pct_shown = status_pct;
      }
      if (check_mode) {
        if (rotary_button_pressed()) {
          status_msg("Test aborted");
          return FR_NOT_READY;
        }
      } else if (!gcode_draw(&gcode_data, servo_calibration)) {
          // something went wrong here, assume that gcode_draw set status
          // for the user.
          return FR_NOT_READY;
      }      
    }
  }

  if (!gcode_data.eof) {
      status_msg_on(ERROR_ROW, "GCODE: NO EOF");
      return FR_NOT_READY;
  }

  if (!check_mode) {
    // Head home
    gcode_data.x = HOME_X;
    gcode_data.y = HOME_Y;
    gcode_data.pen_is_down = FALSE;
    gcode_draw(&gcode_data, servo_calibration);
  }

  status_msg("Finished");
  return FR_OK;
}

void gcode_file_draw(uint32_t file_size, bool_t check_mode) {
  // the user likely is still pressing the rotary button, wait for it
  // to be released
  while (rotary_button_pressed()) {
      _delay_ms(15);
  }

  status_start_on(7, "Press button to abort");

  const struct ServoCalibration* servo_calibration =
    check_mode ? 0 : servo_common_init();
  process_gcode_file(servo_calibration, file_size);
}