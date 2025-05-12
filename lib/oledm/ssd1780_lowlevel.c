#include "ssd1780_lowlevel.h"
#include "ssd1780_commands.h"
#include "oledm.h"

#include <util/delay.h>

extern void oledm_startCommands(error_t* err);
extern void oledm_command(uint8_t cmd, error_t* err);

error_t ssd1780_err = 0;

void ssd1780_start(void) {
  oledm_startCommands(&ssd1780_err);
}

void ssd1780_stop(void) {
  oledm_stop(0);  // display==NULL is OK for SPI because it's ignored.
}

void ssd1780_set_column_address(column_t start, column_t end) {
  oledm_command(SSD1780_SET_COLUMN_STARTEND, &ssd1780_err);
  oledm_command(start, &ssd1780_err);
  oledm_command(end, &ssd1780_err);
}

void ssd1780_set_row_address(uint8_t start, uint8_t end) {
  oledm_command(SSD1780_SET_ROW_STARTEND, &ssd1780_err);
  oledm_command(start, &ssd1780_err);
  oledm_command(end, &ssd1780_err);
}

void ssd1780_set_contrast_color_a(uint8_t contrast) {
  oledm_command(SSD1780_SET_CONTRAST_A, &ssd1780_err);
  oledm_command(contrast, &ssd1780_err);
}

void ssd1780_set_contrast_color_b(uint8_t contrast) {
  oledm_command(SSD1780_SET_CONTRAST_B, &ssd1780_err);
  oledm_command(contrast, &ssd1780_err);
}

void ssd1780_set_contrast_color_c(uint8_t contrast) {
  oledm_command(SSD1780_SET_CONTRAST_C, &ssd1780_err);
  oledm_command(contrast, &ssd1780_err);
}

void ssd1780_master_current_control(uint8_t attenuation) {
  oledm_command(SSD1780_MASTER_CURRENT, &ssd1780_err);
  oledm_command(attenuation, &ssd1780_err);
}

void ssd1780_set_second_precharge_speed(
    uint8_t color_a, uint8_t color_b, uint8_t color_c) {
  oledm_command(SSD1780_PRECHARGE_A, &ssd1780_err);
  oledm_command(color_a, &ssd1780_err);
  oledm_command(SSD1780_PRECHARGE_B, &ssd1780_err);
  oledm_command(color_b, &ssd1780_err);
  oledm_command(SSD1780_PRECHARGE_C, &ssd1780_err);
  oledm_command(color_c, &ssd1780_err);
}

void ssd1780_remap_and_color_depth(SSD1780_RemapFlags flags) {
  oledm_command(SSD1780_REMAP_AND_COLOR_DEPTH, &ssd1780_err);
  oledm_command(flags, &ssd1780_err);
}

void ssd1780_set_display_start_line(uint8_t start_line) {
  oledm_command(SSD1780_SET_START_LINE, &ssd1780_err);
  oledm_command(start_line, &ssd1780_err);
}

void ssd1780_set_display_offset(uint8_t offset) {
  oledm_command(SSD1780_SET_DISPLAY_OFFSET, &ssd1780_err);
  oledm_command(offset, &ssd1780_err);
}

void ssd1780_set_display_mode(SSD1780_DisplayMode mode) {
  oledm_command(SSD1780_SET_DISPLAY_MODE | mode, &ssd1780_err);
}

void ssd1780_set_multiplex_ratio(uint8_t ratio) {
  oledm_command(SSD1780_SET_MULTIPLEX_RATIO, &ssd1780_err);
  oledm_command(ratio, &ssd1780_err);
}

void ssd1780_dim_mode_settings(
    uint8_t contrast_a,
    uint8_t contrast_b,
    uint8_t contrast_c,
    uint8_t precharge_voltage) {
  oledm_command(SSD1780_DIM_MODE_SETTING, &ssd1780_err);
  oledm_command(0x00, &ssd1780_err);
  oledm_command(contrast_a, &ssd1780_err);
  oledm_command(contrast_b, &ssd1780_err);
  oledm_command(contrast_c, &ssd1780_err);
  oledm_command(precharge_voltage, &ssd1780_err);
}

void ssd1780_set_master_configuration(void) {
  oledm_command(SSD1780_SET_MASTER_CONFIGURATION, &ssd1780_err);
  oledm_command(0x00, &ssd1780_err);
}

void ssd1780_set_display_on_off(SSD1780_DisplayOnOff mode) {
  oledm_command(SSD1780_DISPLAY_DIM | mode, &ssd1780_err);
}

void ssd1780_power_save_mode(SSD1780_PowerSaveMode mode) {
  oledm_command(SSD1780_POWER_SAVE_MODE, &ssd1780_err);
  oledm_command(mode, &ssd1780_err);
}

void ssd1780_phase_1_2_period_adjust(
    uint8_t phase1_period, uint8_t phase2_period) {
  oledm_command(SSD1780_PHASE_1_2_PERIOD, &ssd1780_err);
  oledm_command((phase2_period << 4) | phase1_period, &ssd1780_err);
}

void ssd1780_clock_divider_fequency(
    uint8_t divide_ratio, uint8_t fosc_freq) {
  oledm_command(SSD1780_DISPLAY_CLOCK_DIVIDER, &ssd1780_err);
  oledm_command((fosc_freq << 4) | divide_ratio, &ssd1780_err);
}

void ssd1780_set_gray_scale_table(uint8_t pulse_widths[]) {
  oledm_command(SSD1780_SET_GRAY_SCALE_TABLE, &ssd1780_err);
  for (uint8_t i=0; i<32; ++i) {
    oledm_command(pulse_widths[i], &ssd1780_err);
  }
}

void ssd1780_reset_gray_scale_table(void) {
  oledm_command(SSD1780_ENABLE_GRAY_SCALE_TABLE, &ssd1780_err);
}

void ssd1780_set_precharge_level(uint8_t precharge_level) {
  oledm_command(SSD1780_SET_PRECHARGE_LEVEL, &ssd1780_err);
  oledm_command(precharge_level, &ssd1780_err);
}

void ssd1780_com_delelect_voltage(uint8_t arg) {
  oledm_command(SSD1780_SET_COM_DESELECT_VOLTS, &ssd1780_err);
  oledm_command(arg, &ssd1780_err);
}

void ssd1780_command_lock(bool_t lock) {
  oledm_command(SSD1780_SET_COMMAND_LOCK, &ssd1780_err);
  oledm_command(0x12 | (lock ? 0x04 : 0x00), &ssd1780_err);
}

void ssd1780_draw_line(
    column_t x1, uint8_t y1, column_t x2, uint8_t y2, uint16_t color) {
  oledm_command(SSD1780_DRAW_LINE, &ssd1780_err);
  oledm_command(x1, &ssd1780_err);
  oledm_command(y1, &ssd1780_err);
  oledm_command(x2, &ssd1780_err);
  oledm_command(y2, &ssd1780_err);
  oledm_command((color & 0xF800) >> 11, &ssd1780_err);
  oledm_command((color & 0x07E0) >> 5, &ssd1780_err);
  oledm_command((color & 0x001F) << 1, &ssd1780_err);
  _delay_ms(1); 
}

void ssd1780_draw_rect(
    column_t left,
    uint8_t top,
    column_t right,
    uint8_t bottom,
    uint16_t line_color,
    uint16_t fill_color) {
  oledm_command(SSD1780_DRAW_RECTANGLE, &ssd1780_err);
  oledm_command(left, &ssd1780_err);
  oledm_command(top, &ssd1780_err);
  oledm_command(right, &ssd1780_err);
  oledm_command(bottom, &ssd1780_err);
  oledm_command((line_color & 0xF800) >> 11, &ssd1780_err);
  oledm_command((line_color & 0x07E0) >> 5, &ssd1780_err);
  oledm_command((line_color & 0x001F) << 1, &ssd1780_err);
  oledm_command((fill_color & 0xF800) >> 11, &ssd1780_err);
  oledm_command((fill_color & 0x07E0) >> 5, &ssd1780_err);
  oledm_command((fill_color & 0x001F) << 1, &ssd1780_err);
  _delay_ms(1); 
}

void ssd1780_copy_rect(
    column_t left,
    uint8_t top,
    column_t right,
    uint8_t bottom,
    column_t dest_x,
    uint8_t dest_y) {
  oledm_command(SSD1780_COPY, &ssd1780_err);
  oledm_command(left, &ssd1780_err);
  oledm_command(top, &ssd1780_err);
  oledm_command(right, &ssd1780_err);
  oledm_command(bottom, &ssd1780_err);
  oledm_command(dest_x, &ssd1780_err);
  oledm_command(dest_y, &ssd1780_err);
  _delay_ms(1); 
}

void ssd1780_dim_window(
    column_t left,
    uint8_t top,
    column_t right,
    uint8_t bottom) {
  oledm_command(SSD1780_DIM_WINDOW, &ssd1780_err);
  oledm_command(left, &ssd1780_err);
  oledm_command(top, &ssd1780_err);
  oledm_command(right, &ssd1780_err);
  oledm_command(bottom, &ssd1780_err);
}

void ssd1780_clear_window(
    column_t left,
    uint8_t top,
    column_t right,
    uint8_t bottom) {
  oledm_command(SSD1780_CLEAR_WINDOW, &ssd1780_err);
  oledm_command(left, &ssd1780_err);
  oledm_command(top, &ssd1780_err);
  oledm_command(right, &ssd1780_err);
  oledm_command(bottom, &ssd1780_err);
}

void ssd1780_fill_enable(SSD1780_FillEnable flags) {
  oledm_command(SSD1780_FILL_ENABLE, &ssd1780_err);
  oledm_command(flags, &ssd1780_err);
}

void ssd1780_continuous_scroll(
    column_t hoffset,
    uint8_t start_row,
    uint8_t num_rows_h,
    uint8_t num_rows_v,
    SSD1780_ScrollTimeInterval time_interval) {
  oledm_command(SSD1780_CONTINUOUS_SCROLL, &ssd1780_err);
  oledm_command(hoffset, &ssd1780_err);
  oledm_command(start_row, &ssd1780_err);
  oledm_command(num_rows_h, &ssd1780_err);
  oledm_command(num_rows_v, &ssd1780_err);
  oledm_command(time_interval, &ssd1780_err);
}

void ssd1780_deactivate_scrolling(void) {
  oledm_command(SSD1780_DEACTIVATE_SCROLL, &ssd1780_err);
}

void ssd1780_activate_scrolling(void) {
  oledm_command(SSD1780_ACTIVATE_SCROLL, &ssd1780_err);
}
