#ifndef OLEDM_COLOR16
  #define OLEDM_COLOR16
#endif
#include "oledm.h"

#include <util/delay.h>
#include <string.h>
#include <error_codes.h>
#include "oledm_driver_common.inc"
#include "ssd1780_commands.h"

#define DISPLAY_MEMORY_ROWS 64
#define DISPLAY_MEMORY_COLUMNS 96

void oledm_basic_init(struct OLEDM* display) {
  memset(display, 0, sizeof(struct OLEDM));
  display->visible_columns = DISPLAY_MEMORY_COLUMNS;
  display->visible_rows = DISPLAY_MEMORY_ROWS;
  display->memory_columns = DISPLAY_MEMORY_COLUMNS;
  display->memory_rows = DISPLAY_MEMORY_ROWS;

#ifdef OLEDM_EMULATE_MONOCHROME
  display->fg_color = 0xFFFF;
  display->mono_column = 0xFF;  // Special initialization state
#endif
}

void oledm_start(struct OLEDM* display) {
  oledm_ifaceInit();

  error_t* err = &(display->error);

  oledm_startCommands(err);
  oledm_command(SSD1780_DISPLAY_OFF, err);
  
  // As specified in the SSD1780 datasheet
  oledm_command(SSD1780_SET_MASTER_CONFIGURATION, err);
  oledm_command(0x8F, err);

  // Here we go with vertical addressing mode because it makes
  // it possible to emulate the monochrome (SSD1306) interface
  // with less bus traffic (as pixels are sent in 8-pixel vertical rows
  // on that hardware).
  // A[0]: 1: Vertical addressing mode
  const uint8_t a0 = 0b1;
  // A[1]: 0: Columns map from left to right
  const uint8_t a1 = 0b1;
  // A[2]: 0: Normal RGB mode
  const uint8_t a2 = 0b0;
  // A[3-5]: 010  Going for top to bottom updates.
  const uint8_t a35 = 0b110;
  // A[6-7]: 01  16-bit color
  const uint8_t a67 = 0b01;
  // Thus we have 0111 0011 -> 0x73
  oledm_command(SSD1780_REMAP_AND_COLOR_DEPTH, err);
  oledm_command((a67 << 6) | (a35 << 3) | (a2 << 2) | (a1 << 1) | a0, err);

  oledm_command(SSD1780_SET_START_LINE, err);
  oledm_command(display->start_line & 0x3F, err);

  oledm_command(SSD1780_DISPLAY_ON, err);
  oledm_stop(display);
}

void oledm_set_memory_bounds16(
    struct OLEDM* display,
    column_t left_column,
    uint8_t top_row,
    column_t right_column,
    uint8_t bottom_row) {

  error_t* err = &(display->error);

  oledm_startCommands(err);

  oledm_command(SSD1780_SET_COLUMN_STARTEND, err);
  oledm_command(left_column, err);
  oledm_command(right_column, err);

  oledm_command(SSD1780_SET_ROW_STARTEND, err);
  oledm_command(top_row, err);
  oledm_command(bottom_row, err);

  oledm_stop(display);
}

void oledm_write_pixel16(struct OLEDM* display, uint16_t pixel){ 
  oledm_ifaceWriteData(pixel >> 8, &(display->error));
  oledm_ifaceWriteData(pixel & 0xFF, &(display->error));
}

void oledm_vscroll16(struct OLEDM* display, int8_t rows) {
  display->start_line = (display->start_line + rows) & (DISPLAY_MEMORY_ROWS - 1);

  error_t* err = &(display->error);
  oledm_startCommands(err);
  oledm_command(SSD1780_SET_START_LINE, err);
  oledm_command(display->start_line, err);
  oledm_stop(display);
}

void oledm_clear(struct OLEDM* display, uint8_t byte) {
  error_t* err = &(display->error);
  oledm_startCommands(err);
  oledm_command(SSD1780_CLEAR_WINDOW, err);
  oledm_command(0, err);
  oledm_command(0, err);
  oledm_command(display->memory_columns - 1, err);
  oledm_command(display->memory_rows - 1, err);
  oledm_stop(display);
  // A small delay is needed to allow the command to finish.
  _delay_ms(1);
}

void oledm_display_off(struct OLEDM* display) {
  error_t* err = &(display->error);
  oledm_startCommands(err);
  oledm_command(SSD1780_DISPLAY_OFF, err);
  oledm_stop(display);
}

void oledm_display_on(struct OLEDM* display) {
  error_t* err = &(display->error);
  oledm_startCommands(err);
  oledm_command(SSD1780_DISPLAY_ON, err);
  oledm_stop(display);
}


uint16_t rgb16_by_index(uint8_t color_index) {
  uint16_t r = 0x00;
  uint16_t g = 0x00;
  uint16_t b = 0x00;
  switch (color_index & 0x0F) {
    case 0: // black
      if (color_index & 0xF0) {
        r = 31; g = 40; break; // sub in a red green mix
      }
      break;
    case 1: // red
      r = 31; break;
    case 2: // green
      g = 63; break;
    case 3: // blue
      b = 31; break;
    case 4: // yellow
      r = 31; g = 63; break;
    case 5: // magenta
      r = 31; b = 31; break;
    case 6: // cyan
      g = 63; b = 31; break;
    case 7: // orange
      r = 31; g = 31; break;
    case 8: // purple
      r = 15; g = 31; break;
    case 9: // cornflower blue
      g = 40; b = 31; break;
    case 10: // pink
      r = 31; g = 44; b = 22; break;
    case 11: // light green
      r = 22; g = 63; b = 22; break;
    case 12: // light magenta
      r = 31; g = 44; b = 31; break;
    case 13: // light orange
      r = 31; g = 44; b = 10; break;
    case 14: // blue 2
      r = 10; g = 44; b = 31; break;
    case 15: // white
      r = 31; g = 63; b = 31; break;
  }

  // scale factor should be 1-16
  uint16_t scale_factor = 16 - (color_index >> 4);
  r = r * scale_factor / 16;
  g = g * scale_factor / 16;
  b = b * scale_factor / 16;
  return rgb16(r,g,b);
}

#ifdef OLEDM_EMULATE_MONOCHROME
static void set_mono_bounds(struct OLEDM* display) {
  display->mono_column = display->mono_left_column;

  // The monochrome display takes data columns, this is partially
  // enumated here, but only one row at a time can be executed.
  uint8_t row = (display->mono_row * 8 + display->mono_row_offset + display->start_line) & 0x3F;
  oledm_set_memory_bounds16(
      display,
      display->mono_left_column,
      row,
      display->mono_right_column,
      row + 7);
}

void oledm_set_memory_bounds(
    struct OLEDM* display,
    column_t left_column,
    uint8_t top_row,
    column_t right_column,
    uint8_t bottom_row) {

  display->mono_row = top_row;
  display->mono_left_column = left_column;
  display->mono_right_column = right_column;
  display->mono_top_row = top_row;
  display->mono_bottom_row = bottom_row;
  
  set_mono_bounds(display);
}

void oledm_write_pixels(struct OLEDM* display, uint8_t byte){
  if (display->mono_column == 0xFF) {
    // The caller is using the assumed default bounds
    oledm_stop(display);
    oledm_set_memory_bounds(
        display,
        0,
        0,
        DISPLAY_MEMORY_COLUMNS - 1,
        (DISPLAY_MEMORY_ROWS / 8) - 1);
    oledm_start_pixels(display);
  }

  for (uint8_t mask = 0x01; mask; mask <<= 1) {
    oledm_write_pixel16(
        display,
        byte & mask ? display->fg_color : display->bg_color
    );
  }

  ++display->mono_column;
  if (display->mono_column > display->mono_right_column) {
    ++display->mono_row;
    if (display->mono_row > display->mono_bottom_row) {
      display->mono_row = display->mono_top_row;
    }
    oledm_stop(display);
    set_mono_bounds(display);
    oledm_start_pixels(display);
  }
}

void oledm_vscroll(struct OLEDM* display, int8_t rows) {
  oledm_vscroll16(display, rows * 8);
}
#endif
