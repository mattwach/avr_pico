#include "text.h"
#include <twi/twi_fake.h>

#include <test/unit_test.h>

// Directly include some deps to avoid making the test makefile more complex
#include "ssd1306.c"
#include "oledm_i2c.c"
#include <twi/twi_fake.c>

static struct FontASCII font = {
    .id = { 'F', 'A', 'S', '1' },
    .first_char = 'H',
    .last_char = 'I',
    .width = 3,
    .height = 2,
    .data = {
      0xFF, 0x00, 0xFF,  // H
      0xFF, 0x01, 0xFF,
      0x01, 0xFF, 0x01,  // I
      0x80, 0xFF, 0x80,
    },
};

void test_init(void) {
    struct OLEDM display;
    struct Text text;
    oledm_basic_init(&display);
    twi_log_reset();
    text_init(&text, &font, &display);
    assert_int_equal(0, display.error);
    assert_int_equal(0, text.row);
    assert_int_equal(0, text.column);
    assert_int_equal(0, (uint8_t*)text.font - (uint8_t*)&font);
    assert_int_equal(0, text.display - &display);
    assert_int_equal(0, twi_logidx);
}

void test_str(void) {
    struct OLEDM display;
    struct Text text;
    oledm_basic_init(&display);
    text_init(&text, &font, &display);
    twi_log_reset();

    text.column = 16;
    text.row = 3;
    text_str(&text, "HI");

    assert_int_equal(0, display.error);

    assert_int_equal(22, text.column);
    assert_int_equal(3, text.row);
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set column address
            TWI_WRITE_NO_STOP, 0x21,
            TWI_WRITE_NO_STOP, 0x80,  // left column
            TWI_WRITE_NO_STOP, 16,
            TWI_WRITE_NO_STOP, 0x80,  // right column
            TWI_WRITE_NO_STOP, 16 + (3 * 2) - 1,
            TWI_WRITE_NO_STOP, 0x80,  // set page address
            TWI_WRITE_NO_STOP, 0x22,
            TWI_WRITE_NO_STOP, 0x80,  // top row
            TWI_WRITE_NO_STOP, 3,
            TWI_WRITE_NO_STOP, 0x80,  // bottom row
            TWI_WRITE_NO_STOP, 4,
            TWI_STOP,

            TWI_START_WRITE, 0x3C,  // Start data
            TWI_WRITE_NO_STOP, 0x40,

            TWI_WRITE_NO_STOP, 0xFF,  // Top of H
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0xFF,
            TWI_WRITE_NO_STOP, 0x01,  // Top of I
            TWI_WRITE_NO_STOP, 0xFF,
            TWI_WRITE_NO_STOP, 0x01,
            TWI_WRITE_NO_STOP, 0xFF,  // Bottom of H
            TWI_WRITE_NO_STOP, 0x01,
            TWI_WRITE_NO_STOP, 0xFF,
            TWI_WRITE_NO_STOP, 0x80,  // Bottom of I
            TWI_WRITE_NO_STOP, 0xFF,
            TWI_WRITE_NO_STOP, 0x80,

            TWI_STOP,  // All done
        }),
        twi_log,
        twi_logidx
    );
}

void test_str_Edge(void) {
    struct OLEDM display;
    struct Text text;
    oledm_basic_init(&display);
    text_init(&text, &font, &display);
    twi_log_reset();

    text.column = 126;
    text.row = 7;
    text_str(&text, "HI");

    assert_int_equal(0, display.error);

    assert_int_equal(128, text.column);
    assert_int_equal(7, text.row);
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set column address
            TWI_WRITE_NO_STOP, 0x21,
            TWI_WRITE_NO_STOP, 0x80,  // left column
            TWI_WRITE_NO_STOP, 126,
            TWI_WRITE_NO_STOP, 0x80,  // right column
            TWI_WRITE_NO_STOP, 127,
            TWI_WRITE_NO_STOP, 0x80,  // set page address
            TWI_WRITE_NO_STOP, 0x22,
            TWI_WRITE_NO_STOP, 0x80,  // top row
            TWI_WRITE_NO_STOP, 7,
            TWI_WRITE_NO_STOP, 0x80,  // bottom row
            TWI_WRITE_NO_STOP, 7,
            TWI_STOP,

            TWI_START_WRITE, 0x3C,  // Start data
            TWI_WRITE_NO_STOP, 0x40,

            TWI_WRITE_NO_STOP, 0xFF,  // Top of H
            TWI_WRITE_NO_STOP, 0x00,

            TWI_STOP,  // All done
        }),
        twi_log,
        twi_logidx
    );
}

void test_str_OffRight(void) {
    struct OLEDM display;
    struct Text text;
    oledm_basic_init(&display);
    text_init(&text, &font, &display);
    twi_log_reset();

    text.column = 128;
    text.row = 0;
    text_str(&text, "HI");

    assert_int_equal(0, display.error);

    assert_int_equal(128, text.column);
    assert_int_equal(0, text.row);
    assert_int_equal(0, twi_logidx);
}

void test_str_OffBottom(void) {
    struct OLEDM display;
    struct Text text;
    oledm_basic_init(&display);
    text_init(&text, &font, &display);
    twi_log_reset();

    text.column = 0;
    text.row = 8;
    text_str(&text, "HI");

    assert_int_equal(0, display.error);

    assert_int_equal(0, text.column);
    assert_int_equal(8, text.row);
    assert_int_equal(0, twi_logidx);
}

void test_str_Empty(void) {
    struct OLEDM display;
    struct Text text;
    oledm_basic_init(&display);
    text_init(&text, &font, &display);
    twi_log_reset();

    text.column = 1;
    text.row = 2;
    text_str(&text, "");

    assert_int_equal(0, display.error);

    assert_int_equal(1, text.column);
    assert_int_equal(2, text.row);
    assert_int_equal(0, twi_logidx);
}

void test_str_preerr(void) {
    struct OLEDM display;
    struct Text text;
    oledm_basic_init(&display);
    text_init(&text, &font, &display);
    twi_log_reset();
    display.error = 0x23;

    text.column = 1;
    text.row = 2;
    text_str(&text, "Hi");

    assert_int_equal(0x23, display.error);

    assert_int_equal(1, text.column);
    assert_int_equal(2, text.row);
    assert_int_equal(0, twi_logidx);
}

void test_clear_row(void) {
    struct OLEDM display;
    struct Text text;
    oledm_basic_init(&display);
    text_init(&text, &font, &display);
    twi_log_reset();

    text.column = 125;
    text.row = 3;
    text_clear_row(&text);

    assert_int_equal(0, display.error);

    assert_int_equal(128, text.column);
    assert_int_equal(3, text.row);
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x3C,
            TWI_WRITE_NO_STOP, 0x80,  // Set column address
            TWI_WRITE_NO_STOP, 0x21,
            TWI_WRITE_NO_STOP, 0x80,  // left column
            TWI_WRITE_NO_STOP, 125,
            TWI_WRITE_NO_STOP, 0x80,  // right column
            TWI_WRITE_NO_STOP, 127,
            TWI_WRITE_NO_STOP, 0x80,  // set page address
            TWI_WRITE_NO_STOP, 0x22,
            TWI_WRITE_NO_STOP, 0x80,  // top row
            TWI_WRITE_NO_STOP, 3,
            TWI_WRITE_NO_STOP, 0x80,  // bottom row
            TWI_WRITE_NO_STOP, 4,
            TWI_STOP,

            TWI_START_WRITE, 0x3C,  // Start data
            TWI_WRITE_NO_STOP, 0x40,

            TWI_WRITE_NO_STOP, 0x00, // 3x2 rows of zeros
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x00,

            TWI_STOP,  // All done
        }),
        twi_log,
        twi_logidx
    );
}

int main(void) {
    test(test_init);
    test(test_str);
    test(test_str_Edge);
    test(test_str_OffRight);
    test(test_str_OffBottom);
    test(test_str_Empty);
    test(test_str_preerr);
    test(test_clear_row);

    return 0;
}
