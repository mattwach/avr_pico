#include "ssd1306_init.h"

#include <test/unit_test.h>

void test_64x32_a(void) {
    struct OLEDM display;
    ssd1306_64x32_a_init(&display);

    assert_int_equal(0, display.error);

    assert_int_equal(64, display.visible_columns);
    assert_int_equal(4, display.visible_rows);

    assert_int_equal(128, display.memory_columns);
    assert_int_equal(8, display.memory_rows);

    assert_int_equal(32, display.column_offset);
    assert_int_equal(4, display.row_offset);

    assert_int_equal(0, display.start_line);

    assert_int_equal(0x02, display.option_bits);
}

void test_128x32_a(void) {
    struct OLEDM display;
    ssd1306_128x32_a_init(&display);

    assert_int_equal(0, display.error);

    assert_int_equal(128, display.visible_columns);
    assert_int_equal(4, display.visible_rows);

    assert_int_equal(128, display.memory_columns);
    assert_int_equal(8, display.memory_rows);

    assert_int_equal(0, display.column_offset);
    assert_int_equal(0, display.row_offset);

    assert_int_equal(0, display.start_line);

    assert_int_equal(0x01, display.option_bits);
}

int main(void) {
    test(test_64x32_a);
    test(test_128x32_a);

    return 0;
}