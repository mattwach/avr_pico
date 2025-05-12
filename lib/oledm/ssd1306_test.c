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
    assert_int_equal(128, display.memory_columns);
    assert_int_equal(8, display.visible_rows);
    assert_int_equal(0, display.start_line);
    assert_int_equal(0, display.error);
    assert_int_equal(2, display.option_bits);
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
            TWI_WRITE_NO_STOP, 0x80,  // horizontal mem mode
            TWI_WRITE_NO_STOP, 0x20,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x00,
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
            TWI_WRITE_NO_STOP, 0x80,  // charge pump
            TWI_WRITE_NO_STOP, 0x8D,
            TWI_WRITE_NO_STOP, 0x80,
            TWI_WRITE_NO_STOP, 0x14,
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
            TWI_WRITE_NO_STOP, 0x80,  // Set column address
            TWI_WRITE_NO_STOP, 0x21,
            TWI_WRITE_NO_STOP, 0x80,  // left column
            TWI_WRITE_NO_STOP, 32,
            TWI_WRITE_NO_STOP, 0x80,  // right column
            TWI_WRITE_NO_STOP, 55,
            TWI_WRITE_NO_STOP, 0x80,  // set page address
            TWI_WRITE_NO_STOP, 0x22,
            TWI_WRITE_NO_STOP, 0x80,  // top row
            TWI_WRITE_NO_STOP, 3,
            TWI_WRITE_NO_STOP, 0x80,  // bottom row
            TWI_WRITE_NO_STOP, 4,
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
            TWI_WRITE_NO_STOP, 0x80,  // Set column address
            TWI_WRITE_NO_STOP, 0x21,
            TWI_WRITE_NO_STOP, 0x80,  // left column
            TWI_WRITE_NO_STOP, 32,
            TWI_WRITE_NO_STOP, 0x80,  // right column
            TWI_WRITE_NO_STOP, 55,
            TWI_WRITE_NO_STOP, 0x80,  // set page address
            TWI_WRITE_NO_STOP, 0x22,
            TWI_WRITE_NO_STOP, 0x80,  // top row (with start_line=1)
            TWI_WRITE_NO_STOP, 7,
            TWI_WRITE_NO_STOP, 0x80,  // bottom row (with start_line=1)
            TWI_WRITE_NO_STOP, 0,
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
            TWI_WRITE_NO_STOP, 0x80,  // Set column address
            TWI_WRITE_NO_STOP, 0x21,
            TWI_WRITE_NO_STOP, 0x80,  // left column
            TWI_WRITE_NO_STOP, 34,
            TWI_WRITE_NO_STOP, 0x80,  // right column
            TWI_WRITE_NO_STOP, 57,
            TWI_WRITE_NO_STOP, 0x80,  // set page address
            TWI_WRITE_NO_STOP, 0x22,
            TWI_WRITE_NO_STOP, 0x80,  // top row (with start_line=1)
            TWI_WRITE_NO_STOP, 2,
            TWI_WRITE_NO_STOP, 0x80,  // bottom row (with start_line=1)
            TWI_WRITE_NO_STOP, 3,
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
            TWI_WRITE_NO_STOP, 0x80,  // Set column address
            TWI_WRITE_NO_STOP, 0x21,
            TWI_WRITE_NO_STOP, 0x80,  // left column
            TWI_WRITE_NO_STOP, 32,
            TWI_WRITE_NO_STOP, 0x80,  // right column
            TWI_WRITE_NO_STOP, 55,
            TWI_WRITE_NO_STOP, 0x80,  // set page address
            TWI_WRITE_NO_STOP, 0x22,
            TWI_WRITE_NO_STOP, 0x80,  // top row
            TWI_WRITE_NO_STOP, 6,
            TWI_WRITE_NO_STOP, 0x80,  // bottom row
            TWI_WRITE_NO_STOP, 7,
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

    oledm_set_memory_bounds(&display, 130, 9, 150, 11);

    assert_int_equal(0, display.error);

    // note that all offsets here are ignored
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set column address
            TWI_WRITE_NO_STOP, 0x21,
            TWI_WRITE_NO_STOP, 0x80,  // left column
            TWI_WRITE_NO_STOP, 2,
            TWI_WRITE_NO_STOP, 0x80,  // right column
            TWI_WRITE_NO_STOP, 22,
            TWI_WRITE_NO_STOP, 0x80,  // set page address
            TWI_WRITE_NO_STOP, 0x22,
            TWI_WRITE_NO_STOP, 0x80,  // top row
            TWI_WRITE_NO_STOP, 1,
            TWI_WRITE_NO_STOP, 0x80,  // bottom row
            TWI_WRITE_NO_STOP, 3,
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

    oledm_set_bounds(&display, 128, 8, 138, 10);

    assert_int_equal(0, display.error);
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set column address
            TWI_WRITE_NO_STOP, 0x21,
            TWI_WRITE_NO_STOP, 0x80,  // left column
            TWI_WRITE_NO_STOP, 0,
            TWI_WRITE_NO_STOP, 0x80,  // right column
            TWI_WRITE_NO_STOP, 10,
            TWI_WRITE_NO_STOP, 0x80,  // set page address
            TWI_WRITE_NO_STOP, 0x22,
            TWI_WRITE_NO_STOP, 0x80,  // top row
            TWI_WRITE_NO_STOP, 0,
            TWI_WRITE_NO_STOP, 0x80,  // bottom row
            TWI_WRITE_NO_STOP, 2,
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
    const uint8_t prefix[] = {
        TWI_START_WRITE, 0x3C,
        TWI_WRITE_NO_STOP, 0x80,  // Set column address
        TWI_WRITE_NO_STOP, 0x21,
        TWI_WRITE_NO_STOP, 0x80,  // left column
        TWI_WRITE_NO_STOP, 0,
        TWI_WRITE_NO_STOP, 0x80,  // right column
        TWI_WRITE_NO_STOP, 127,
        TWI_WRITE_NO_STOP, 0x80,  // set page address
        TWI_WRITE_NO_STOP, 0x22,
        TWI_WRITE_NO_STOP, 0x80,  // top row
        TWI_WRITE_NO_STOP, 0,
        TWI_WRITE_NO_STOP, 0x80,  // bottom row
        TWI_WRITE_NO_STOP, 7,
        TWI_STOP,
        TWI_START_WRITE, 0x3C,  // start data
        TWI_WRITE_NO_STOP, 0x40,
    };
    memcpy(expected_next, prefix, sizeof(prefix));
    expected_next += sizeof(prefix);

    const int pixel_count = 128 * 8;
    int i=0;
    for (; i<pixel_count; ++i) {
        *(expected_next++) = TWI_WRITE_NO_STOP;
        *(expected_next++) = 0x00;
    }
    *(expected_next++) = TWI_STOP;

    assert_int_equal(expected_next - expected, twi_logidx);
    assert_buff_equal(expected, twi_log, twi_logidx);
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
