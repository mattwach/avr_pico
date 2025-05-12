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

    assert_int_equal(128, display.visible_columns);
    assert_int_equal(8, display.visible_rows);
    assert_int_equal(132, display.memory_columns);
    assert_int_equal(8, display.visible_rows);
    assert_int_equal(0, display.start_line);
    assert_int_equal(0, display.error);
    assert_int_equal(0x02, display.option_bits);
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
            TWI_WRITE_NO_STOP, 0x80,  // Display off
            TWI_WRITE_NO_STOP, 0xAE,
            TWI_WRITE_NO_STOP, 0x80,  // comscandec
            TWI_WRITE_NO_STOP, 0xC8,
            TWI_WRITE_NO_STOP, 0x80,  // setstartline
            TWI_WRITE_NO_STOP, 0x40,
            TWI_WRITE_NO_STOP, 0x80,  // segremap
            TWI_WRITE_NO_STOP, 0xA1,
            TWI_WRITE_NO_STOP, 0x80,  // setcompins
            TWI_WRITE_NO_STOP, 0xDA,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x12,
            TWI_WRITE_NO_STOP, 0x80,  // display on resume
            TWI_WRITE_NO_STOP, 0xA4,
            TWI_WRITE_NO_STOP, 0x80,  // display on
            TWI_WRITE_NO_STOP, 0xAF,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_setBounds(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_set_bounds(&display, 32, 3, 55, 4);

    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set page
            TWI_WRITE_NO_STOP, 0xB3,
            TWI_WRITE_NO_STOP, 0x80,  // set low column
            TWI_WRITE_NO_STOP, 0x02,
            TWI_WRITE_NO_STOP, 0x80,  // set high column
            TWI_WRITE_NO_STOP, 0x12,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_setBounds_withStartline(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.start_line = 1;

    oledm_set_bounds(&display, 32, 6, 55, 7);

    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set page
            TWI_WRITE_NO_STOP, 0xB7,
            TWI_WRITE_NO_STOP, 0x80,  // set low column
            TWI_WRITE_NO_STOP, 0x02,
            TWI_WRITE_NO_STOP, 0x80,  // set high column
            TWI_WRITE_NO_STOP, 0x12,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_setBounds_withOffsets(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.start_line = 1;
    display.column_offset = 2;
    display.row_offset = 3;

    oledm_set_bounds(&display, 32, 6, 55, 7);

    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set page
            TWI_WRITE_NO_STOP, 0xB2,
            TWI_WRITE_NO_STOP, 0x80,  // set low column
            TWI_WRITE_NO_STOP, 0x02,
            TWI_WRITE_NO_STOP, 0x80,  // set high column
            TWI_WRITE_NO_STOP, 0x12,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_setMemoryBounds(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.start_line = 1;
    display.column_offset = 2;
    display.row_offset = 3;

    oledm_set_memory_bounds(&display, 32, 6, 55, 7);

    assert_int_equal(0, display.error);

    // note that all offsets here are ignored
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set page
            TWI_WRITE_NO_STOP, 0xB6,
            TWI_WRITE_NO_STOP, 0x80,  // set low column
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x80,  // set high column
            TWI_WRITE_NO_STOP, 0x12,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_setMemoryBounds_highVals(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.start_line = 1;
    display.column_offset = 2;
    display.row_offset = 3;

    oledm_set_memory_bounds(&display, 145, 9, 150, 11);

    assert_int_equal(0, display.error);

    // note that all offsets here are ignored
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set page
            TWI_WRITE_NO_STOP, 0xB1,
            TWI_WRITE_NO_STOP, 0x80,  // set low column
            TWI_WRITE_NO_STOP, 0x0D,
            TWI_WRITE_NO_STOP, 0x80,  // set high column
            TWI_WRITE_NO_STOP, 0x10,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_setBounds_highVals(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_set_bounds(&display, 145, 8, 150, 10);

    assert_int_equal(0, display.error);
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set page
            TWI_WRITE_NO_STOP, 0xB0,
            TWI_WRITE_NO_STOP, 0x80,  // set low column
            TWI_WRITE_NO_STOP, 0x0F,
            TWI_WRITE_NO_STOP, 0x80,  // set high column
            TWI_WRITE_NO_STOP, 0x10,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_setBounds_i2c_err(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    twi_queue_err(0x23);

    oledm_set_bounds(&display, 32, 1, 55, 4);

    assert_int_equal(0x23, display.error);
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
        }),
        twi_log,
        twi_logidx
    );
}

void test_setBounds_preerr(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.error = 0x34;

    oledm_set_bounds(&display, 32, 1, 55, 4);

    assert_int_equal(0x34, display.error);
    assert_int_equal(0, twi_logidx);
}

void test_vscroll_positive(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_vscroll(&display, 2);

    assert_int_equal(0, display.error);
    assert_int_equal(2, display.start_line);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set start line
            TWI_WRITE_NO_STOP, 0x40 | 16,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_vscroll_positive2(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    display.start_line = 6;
    twi_log_reset();

    oledm_vscroll(&display, 2);

    assert_int_equal(0, display.error);
    assert_int_equal(0, display.start_line);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set start line
            TWI_WRITE_NO_STOP, 0x40,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_vscroll_negative(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_vscroll(&display, -2);

    assert_int_equal(0, display.error);
    assert_int_equal(6, display.start_line);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set start line
            TWI_WRITE_NO_STOP, 0x40 | 48,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_vscroll_negative2(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    display.start_line = 7;
    twi_log_reset();

    oledm_vscroll(&display, -2);

    assert_int_equal(0, display.error);
    assert_int_equal(5, display.start_line);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set start line
            TWI_WRITE_NO_STOP, 0x40 | 40,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_vscroll_i2c_err(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    twi_queue_err(0x23);

    oledm_vscroll(&display, 2);

    assert_int_equal(0x23, display.error);
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
        }),
        twi_log,
        twi_logidx
    );
}

void test_vscroll_preerr(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.error = 0x34;

    oledm_vscroll(&display, 2);

    assert_int_equal(0x34, display.error);
    assert_int_equal(0, twi_logidx);
}

void test_startData(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_start_pixels(&display);

    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x40,
        }),
        twi_log,
        twi_logidx
    );
}

void test_startData_i2c_err(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    twi_queue_err(0x23);

    oledm_start_pixels(&display);

    assert_int_equal(0x23, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
        }),
        twi_log,
        twi_logidx
    );
}

void test_startData_preerr(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.error = 0x34;

    oledm_start_pixels(&display);

    assert_int_equal(0x34, display.error);
    assert_int_equal(0, twi_logidx);
}

void test_stopData(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_stop(&display);

    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){ TWI_STOP }),
        twi_log,
        twi_logidx
    );
}

void test_stopData_i2c_err(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    twi_queue_err(0x23);

    oledm_stop(&display);

    assert_int_equal(0x23, display.error);

    assert_buff_equal(
        ((uint8_t[]){ TWI_STOP }),
        twi_log,
        twi_logidx
    );
}

void test_stopData_preerr(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.error = 0x34;

    oledm_stop(&display);

    assert_int_equal(0x34, display.error);
    assert_int_equal(0, twi_logidx);
}

void test_write_data_no_stop(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_write_pixels(&display, 0xAE);

    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_WRITE_NO_STOP, 0xAE,
        }),
        twi_log,
        twi_logidx
    );
}

void test_write_data_no_stop_wrap(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_set_bounds(&display, 46, 3, 47, 4);
    oledm_start_pixels(&display);
    oledm_write_pixels(&display, 0xE1);  // 46,3
    oledm_write_pixels(&display, 0xE2);  // 47,3
    oledm_write_pixels(&display, 0xE3);  // 46,4
    oledm_write_pixels(&display, 0xE4);  // 47,4
    oledm_write_pixels(&display, 0xE5);  // 46,3
    oledm_write_pixels(&display, 0xE6);  // 47,3
    oledm_stop(&display);

    assert_int_equal(0, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set page
            TWI_WRITE_NO_STOP, 0xB3,
            TWI_WRITE_NO_STOP, 0x80,  // set low column
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x80,  // set high column
            TWI_WRITE_NO_STOP, 0x13,
            TWI_STOP,

            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x40, // Start data
            TWI_WRITE_NO_STOP, 0xE1, // 46,3
            TWI_WRITE_NO_STOP, 0xE2, // 47,3
            TWI_STOP,

            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set page
            TWI_WRITE_NO_STOP, 0xB4,
            TWI_WRITE_NO_STOP, 0x80,  // set low column
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x80,  // set high column
            TWI_WRITE_NO_STOP, 0x13,
            TWI_STOP,

            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x40, // Start data
            TWI_WRITE_NO_STOP, 0xE3, // 46,4
            TWI_WRITE_NO_STOP, 0xE4, // 47,4
            TWI_STOP,

            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set page
            TWI_WRITE_NO_STOP, 0xB3,
            TWI_WRITE_NO_STOP, 0x80,  // set low column
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x80,  // set high column
            TWI_WRITE_NO_STOP, 0x13,
            TWI_STOP,

            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x40, // Start data
            TWI_WRITE_NO_STOP, 0xE5, // 46,3
            TWI_WRITE_NO_STOP, 0xE6, // 47,3
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_write_data_no_stop_i2c_err(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    twi_queue_err(0x23);

    oledm_write_pixels(&display, 0xAE);

    assert_int_equal(0x23, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_WRITE_NO_STOP, 0xAE,
        }),
        twi_log,
        twi_logidx
    );
}

void test_write_data_no_stop_preerr(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.error = 0x34;

    oledm_write_pixels(&display, 0xAE);

    assert_int_equal(0x34, display.error);
    assert_int_equal(0, twi_logidx);
}

void test_clear(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();

    oledm_clear(&display, 0x00);

    assert_int_equal(0, display.error);

    uint8_t expected[4096];
    uint8_t* expected_next = expected;
    int row = 0;
    for (; row < 8; ++row) {
      *(expected_next++) = TWI_START_WRITE;
      *(expected_next++) = 0x3C;
      *(expected_next++) = TWI_WRITE_NO_STOP;
      *(expected_next++) = 0x80;
      *(expected_next++) = TWI_WRITE_NO_STOP;  // Set row
      *(expected_next++) = 0xB0 | row;
      *(expected_next++) = TWI_WRITE_NO_STOP;
      *(expected_next++) = 0x80;
      *(expected_next++) = TWI_WRITE_NO_STOP;  // set low column
      *(expected_next++) = 0x03;
      *(expected_next++) = TWI_WRITE_NO_STOP;
      *(expected_next++) = 0x80;
      *(expected_next++) = TWI_WRITE_NO_STOP; // set high column
      *(expected_next++) = 0x18;
      *(expected_next++) = TWI_STOP;

      *(expected_next++) = TWI_START_WRITE;
      *(expected_next++) = 0x3C;
      *(expected_next++) = TWI_WRITE_NO_STOP;
      *(expected_next++) = 0x40; // Start data
      int col = 0;
      for (; col < 132; ++col) {
        *(expected_next++) = TWI_WRITE_NO_STOP;
        *(expected_next++) = 0x00;  // fill in the data
      }
      *(expected_next++) = TWI_STOP;
    }

    assert_buff_equal(expected, twi_log, twi_logidx);
    assert_int_equal(expected_next - expected, twi_logidx);
}

void test_clear_i2c_err(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    twi_queue_err(0x23);

    oledm_clear(&display, 0x00);

    assert_int_equal(0x23, display.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
        }),
        twi_log,
        twi_logidx
    );
}

void test_clear_preerr(void) {
    struct OLEDM display;
    oledm_basic_init(&display);
    twi_log_reset();
    display.error = 0x34;

    oledm_clear(&display, 0x00);

    assert_int_equal(0x34, display.error);
    assert_int_equal(0, twi_logidx);
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
            TWI_WRITE_NO_STOP, 0xAE,  // Display off
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
            TWI_WRITE_NO_STOP, 0xAF,  // Display on
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

int main(void) {
    test(test_basic_init);
    test(test_start);

    test(test_setBounds);
    test(test_setBounds_withStartline);
    test(test_setBounds_withOffsets);
    test(test_setBounds_highVals);
    test(test_setBounds_i2c_err);
    test(test_setBounds_preerr);

    test(test_setMemoryBounds);
    test(test_setMemoryBounds_highVals);

    test(test_startData);
    test(test_startData);
    test(test_startData_i2c_err);
    test(test_startData_preerr);

    test(test_stopData);
    test(test_stopData_i2c_err);
    test(test_stopData_preerr);

    test(test_write_data_no_stop);
    test(test_write_data_no_stop_wrap);
    test(test_write_data_no_stop_i2c_err);
    test(test_write_data_no_stop_preerr);

    test(test_clear);
    test(test_clear_i2c_err);
    test(test_clear_preerr);

    test(test_vscroll_positive);
    test(test_vscroll_positive2);
    test(test_vscroll_negative);
    test(test_vscroll_negative2);
    test(test_vscroll_i2c_err);
    test(test_vscroll_preerr);

    test(test_display_off);
    test(test_display_on);

    return 0;
}
