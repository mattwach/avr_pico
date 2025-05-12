#ifndef OLED_SSD1780_LOW_LEVEL_H
#define OLED_SSD1780_LOW_LEVEL_H

#include "oledm.h"

// Special functions for the ssd1780.
//
// The function in this file are special ones that are specific to the
// SSD1780 hardware and are not generic enough to be included in oledm.h
//
// For basic operations, use the OLEDM interface and do not use this file
// or ssd1780_lowlevel.c as doing so restricts your code to this specifc
// chip.
//
// Where this file comes into play is when you need to use something
// like the accelerated 2D commands or contrast controls.
//
// Check the SSD1780 (or SSD1331) datasheet for descriptions of the
// low level commands below.

// Note, for all of these, you need to use
// ssd1780_start();
// ... commands here ...
// ssd1780_stop();
//
// Check for errors by looking at ssd1780_err and comparing with 0
extern error_t ssd1780_err;
void ssd1780_start(void);
void ssd1780_stop(void);

void ssd1780_set_column_address(column_t start, column_t end);
void ssd1780_set_row_address(uint8_t start, uint8_t end);
void ssd1780_set_contrast_color_a(uint8_t contrast);
void ssd1780_set_contrast_color_b(uint8_t contrast);
void ssd1780_set_contrast_color_c(uint8_t contrast);
void ssd1780_master_current_control(uint8_t attenuation);
void ssd1780_set_second_precharge_speed(
    uint8_t color_a, uint8_t color_b, uint8_t color_c);

typedef enum SSD1780_RemapFlags {
  VERTICAL_ADDRESS_INCREMENT = 0x01,
  REVERSE_COLUMN_MAP         = 0x02,
  BGR_COLOR                  = 0x04,
  LEFT_RIGHT_COM_SWAP        = 0x08,
  REVERSE_COM_SCAN           = 0x10,
  ENABLE_COM_SPLIT_EVEN      = 0x20,
  COLOR_65K                  = 0x40,
  COLOR_65K_FORMAT2          = 0x80,
} SSD1780_RemapFlags;
void ssd1780_remap_and_color_depth(SSD1780_RemapFlags flags);
void ssd1780_set_display_start_line(uint8_t start_line);
void ssd1780_set_display_offset(uint8_t offset);

typedef enum SSD1780_DisplayMode {
  NORMAL_DISPLAY  = 0x00,
  ALL_PIXELS_ON   = 0x01,
  ALL_PIXELS_OFF  = 0x02,
  INVERSE_DISPLAY = 0x03,
} SSD1780_DisplayMode;
void ssd1780_set_display_mode(SSD1780_DisplayMode mode);
void ssd1780_set_multiplex_ratio(uint8_t ratio);
void ssd1780_dim_mode_settings(
    uint8_t contrast_a,
    uint8_t contrast_b,
    uint8_t contrast_c,
    uint8_t precharge_voltage);
void ssd1780_set_master_configuration(void);

typedef enum SSD1780_DisplayOnOff {
  DISPLAY_DIM = 0x00,
  DISPLAY_OFF = 0x02,
  DISPLAY_ON  = 0x03,
} SSD1780_DisplayOnOff;
void ssd1780_set_display_on_off(SSD1780_DisplayOnOff mode);

typedef enum SSD1780_PowerSaveMode {
  ENABLE_POWER_SAVE  = 0x1A,
  DISABLE_POWER_SAVE = 0x0B,
} SSD1780_PowerSaveMode;
void ssd1780_power_save_mode(SSD1780_PowerSaveMode mode);
void ssd1780_phase_1_2_period_adjust(
    uint8_t phase1_period, uint8_t phase2_period);
void ssd1780_clock_divider_fequency(
    uint8_t divide_ratio, uint8_t fosc_freq);
void ssd1780_set_gray_scale_table(uint8_t pulse_widths[]);  // 32 values
void ssd1780_reset_gray_scale_table(void);
void ssd1780_set_precharge_level(uint8_t precharge_level);
void ssd1780_com_delelect_voltage(uint8_t arg);
void ssd1780_command_lock(bool_t lock);
void ssd1780_draw_line(
    column_t x1, uint8_t y1, column_t x2, uint8_t y2, uint16_t color);
void ssd1780_draw_rect(
    column_t left,
    uint8_t top,
    column_t right,
    uint8_t bottom,
    uint16_t line_color,
    uint16_t fill_color);
void ssd1780_copy_rect(
    column_t left,
    uint8_t top,
    column_t right,
    uint8_t bottom,
    column_t dest_x,
    uint8_t dest_y);
void ssd1780_dim_window(
    column_t left,
    uint8_t top,
    column_t right,
    uint8_t bottom);
void ssd1780_clear_window(
    column_t left,
    uint8_t top,
    column_t right,
    uint8_t bottom);

typedef enum SSD1780_FillEnable {
  RECTANGLE_FILL_ENABLE = 0x01,
  REVERSE_COPY_ENABLE   = 0x10,
} SSD1780_FillEnable;
void ssd1780_fill_enable(SSD1780_FillEnable flags);

typedef enum SSD1780_ScrollTimeInterval {
  TIME_INTERVAL_6_FRAMES   = 0x00,
  TIME_INTERVAL_10_FRAMES  = 0x01,
  TIME_INTERVAL_100_FRAMES = 0x02,
  TIME_INTERVAL_200_FRAMES = 0x03,
} SSD1780_ScrollTimeInterval;
void ssd1780_continuous_scroll(
    column_t hoffset,
    uint8_t start_row,
    uint8_t num_rows_h,
    uint8_t num_rows_v,
    SSD1780_ScrollTimeInterval time_interval);
void ssd1780_deactivate_scrolling(void);
void ssd1780_activate_scrolling(void);


#endif
