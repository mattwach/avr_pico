#define OLEDM_COLOR16
#include "oledm.h"
#include <twi/twi_fake.h>

#include <test/unit_test.h>

// Directly include some deps to avoid making the test makefile more complex
#include "oledm_i2c.c"
#include <twi/twi_fake.c>

void test_basic_init(void) {
    struct OLEDM display;
    twi_log_reset();
    oledm_basic_init(&display);

    assert_int_equal(96, display.visible_columns);
    assert_int_equal(64, display.visible_rows);
    assert_int_equal(96, display.memory_columns);
    assert_int_equal(64, display.visible_rows);
    assert_int_equal(0, display.start_line);
    assert_int_equal(0, display.error);
    assert_int_equal(0, display.option_bits);

    assert_int_equal(0x0000, display.bg_color);
    assert_int_equal(0xFFFF, display.fg_color);
    assert_int_equal(0xFF, display.mono_column);
    assert_int_equal(0, display.mono_row);
    assert_int_equal(0, display.mono_left_column);
    assert_int_equal(0, display.mono_right_column);
    assert_int_equal(0, display.mono_top_row);
    assert_int_equal(0, display.mono_bottom_row);

    assert_int_equal(0, twi_logidx);
}

void test_start(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_start(&display);
    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_INIT,
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0xAE,  // SSD1780_DISPLAY_OFF
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0xAD,  // SSD1780_SET_MASTER_CONFIGURATION
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x8F,  // Arg
            TWI_WRITE_NO_STOP, 0x80,  
            TWI_WRITE_NO_STOP, 0xA0,  // SSD1780_REMAP_AND_COLOR_DEPTH
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x73,  // Arg
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0xA1,  // SSD1780_SET_START_LINE
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x00,  // Arg
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0xAF,  // SSD1780_DISPLAY_ON
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_set_memory_bounds16(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_set_memory_bounds16(&display, 1, 2, 3, 4);
    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x15,  // SSD1780_SET_COLUMN_STARTEND
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x01,  // ARG
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x03,  // ARG
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x75,  // SSD1780_SET_ROW_STARTEND
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x02,  // ARG
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x04,  // ARG
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_set_memory_bounds(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_set_memory_bounds(&display, 1, 2, 3, 4);
    assert_int_equal(0, display.error);

    assert_int_equal(1, display.mono_column);
    assert_int_equal(1, display.mono_left_column);
    assert_int_equal(3, display.mono_right_column);
    assert_int_equal(2, display.mono_row);
    assert_int_equal(2, display.mono_top_row);
    assert_int_equal(4, display.mono_bottom_row);
    assert_int_equal(0x0000, display.bg_color);
    assert_int_equal(0xFFFF, display.fg_color);


    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x15,  // SSD1780_SET_COLUMN_STARTEND
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 1,  // ARG
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 3,  // ARG
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x75,  // SSD1780_SET_ROW_STARTEND
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 2 * 8,  // ARG
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 2 * 8 + 7,  // ARG
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_write_pixel16(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_write_pixel16(&display, 0x1234);

    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_WRITE_NO_STOP, 0x12,
            TWI_WRITE_NO_STOP, 0x34,
        }),
        twi_log,
        twi_logidx
    );
}

void test_write_pixels(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    oledm_set_memory_bounds(&display, 1, 3, 2, 4);

    // 0001 0010
    oledm_write_pixels(&display, 0x12);
    assert_int_equal(0, display.error);

    assert_int_equal(2, display.mono_column);
    assert_int_equal(3, display.mono_row);
    assert_int_equal(0x0000, display.bg_color);
    assert_int_equal(0xFFFF, display.fg_color);
    twi_log_reset();

    oledm_write_pixels(&display, 0x12);
    assert_int_equal(0, display.error);
    assert_int_equal(1, display.mono_column);
    assert_int_equal(4, display.mono_row);


    assert_buff_equal(
        ((uint8_t[]){
            // Data
            TWI_WRITE_NO_STOP, 0x00, // Bit 0
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0xFF, // Bit 1
            TWI_WRITE_NO_STOP, 0xFF,
            TWI_WRITE_NO_STOP, 0x00, // Bit 2
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x00, // Bit 3
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0xFF, // Bit 4
            TWI_WRITE_NO_STOP, 0xFF,
            TWI_WRITE_NO_STOP, 0x00, // Bit 5
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x00, // Bit 6
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x00, // Bit 7
            TWI_WRITE_NO_STOP, 0x00,

            TWI_STOP,
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x15,  // SSD1780_SET_COLUMN_STARTEND
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 1,  // ARG
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 2,  // ARG
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x75,  // SSD1780_SET_ROW_STARTEND
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 4 * 8,  // ARG
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 4 * 8 + 7,  // ARG
            TWI_STOP,

            TWI_START_WRITE, 0x3C,  // These are from oledm_start_pixels
            TWI_WRITE_NO_STOP, 0x40,
        }),
        twi_log,
        twi_logidx
    );

    oledm_write_pixels(&display, 0x12);
    assert_int_equal(0, display.error);
    assert_int_equal(2, display.mono_column);
    assert_int_equal(4, display.mono_row);

    oledm_write_pixels(&display, 0x12);
    assert_int_equal(0, display.error);
    assert_int_equal(1, display.mono_column);
    assert_int_equal(3, display.mono_row);
    twi_log_reset();
}

void test_clear(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_clear(&display, 0x00);
    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x25,  // SSD1780_CLEAR_WINDOW
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x00,  // start column
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x00,  // start row
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x5F,  // end column
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x3F,  // end row
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_display_off(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_display_off(&display);
    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0xAE,  // SSD1780_DISPLAY_OFF
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_display_on(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_display_on(&display);
    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0xAF,  // SSD1780_DISPLAY_ON
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_vscroll16(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_vscroll16(&display, 0x12);
    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0xA1,  // SSD1780_SET_START_LINE
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x12,  // Arg
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_vscroll(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_vscroll(&display, 2);
    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0xA1,  // SSD1780_SET_START_LINE
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 8 * 2,  // Arg
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

int main(void) {
    test(test_basic_init);
    test(test_start);
    test(test_set_memory_bounds);
    test(test_set_memory_bounds16);
    test(test_write_pixel16);
    test(test_write_pixels);
    test(test_clear);
    test(test_display_off);
    test(test_display_on);
    test(test_vscroll);
    test(test_vscroll16);

    return 0;
}
