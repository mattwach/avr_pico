#include <test/unit_test.h>

#include "gcode_parser.h"

bool_t feed_data(struct GCodeParseData* d, const char* str, int* idx) {
    while (str[*idx]) {
        bool_t available = gcode_parse(str[*idx], d);
        if (available) {
            ++(*idx);
            return TRUE;
        }
        if (d->error) {
            return FALSE;
        }
        ++(*idx);
    }
    return FALSE;
}

void test_error_code_str(void) {
  assert_str_equal("OK      ", gcode_error_str(0));
  assert_str_equal("NumParse", gcode_error_str(ERR_NUMBER_PARSE));
  assert_str_equal("NumLarge", gcode_error_str(ERR_NUMBER_TOO_LARGE));
  assert_str_equal("FeedHigh", gcode_error_str(ERR_FEED_RATE_OUT_OF_RANGE));
  assert_str_equal("Token?  ", gcode_error_str(ERR_UNEXPECTED_TOKEN));
  assert_str_equal("ScalHigh", gcode_error_str(ERR_SCALE_OUT_OF_RANGE));
  assert_str_equal("NumLong ", gcode_error_str(ERR_NUMBER_TOO_LONG));
  assert_str_equal("BadCode ", gcode_error_str(ERR_BAD_GCODE));
  assert_str_equal("MissCode", gcode_error_str(ERR_MISSING_GCODE));
  assert_str_equal("MissingN", gcode_error_str(ERR_MISSING_LINE_NUMBER));
  assert_str_equal("NSequenc", gcode_error_str(ERR_LINE_NUMBER_SEQUENCE));
  assert_str_equal("NTooHigh", gcode_error_str(ERR_LINE_NUMBER_TOO_LARGE));
  assert_str_equal("Data>EOF", gcode_error_str(ERR_DATA_AFTER_EOF));
  assert_str_equal("Chars?  ", gcode_error_str(ERR_UNEXPECTED_CHARACTERS));
  assert_str_equal("Missing%", gcode_error_str(ERR_MISSING_START_OF_FILE_MARKER));

  assert_int_equal(ERR_MISSING_START_OF_FILE_MARKER, MAX_ERROR_CODE);
}

void test_no_start(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "hello\n", &idx));
    assert_int_equal(0, idx);
    assert_int_equal(ERR_MISSING_START_OF_FILE_MARKER, d.error);
}

void test_start_only(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(
        FALSE,
        feed_data(
            &d,
            ";a comment\n% ;another comment\n;another one\n",
            &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(FALSE, d.eof);
}

void test_data_after_eof(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\n%\nN", &idx));
    assert_int_equal(4, idx);
    assert_int_equal(TRUE, d.eof);
    assert_int_equal(ERR_DATA_AFTER_EOF, d.error);
}

void test_trailing_chars(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%X", &idx));
    assert_int_equal(1, idx);
    assert_int_equal(ERR_UNEXPECTED_CHARACTERS, d.error);
}

void test_no_N(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\n;comment\nG01\n", &idx));
    assert_int_equal(11, idx);
    assert_int_equal(ERR_MISSING_LINE_NUMBER, d.error);
}

void test_bad_N(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN02 ", &idx));
    assert_int_equal(5, idx);
    assert_int_equal(ERR_LINE_NUMBER_SEQUENCE, d.error);
}

void test_lonely_N(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN ", &idx));
    assert_int_equal(3, idx);
    assert_int_equal(ERR_MISSING_LINE_NUMBER, d.error);
}

void test_N_bad_parse(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nNX ", &idx));
    assert_int_equal(4, idx);
    assert_int_equal(ERR_NUMBER_PARSE, d.error);
}

void test_N_long(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN000001", &idx));
    assert_int_equal(8, idx);
    assert_int_equal(ERR_LINE_NUMBER_TOO_LARGE, d.error);
}

void test_no_G(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 K", &idx));
    assert_int_equal(6, idx);
    assert_int_equal(ERR_MISSING_GCODE, d.error);
}

void test_G_parse(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 GK ", &idx));
    assert_int_equal(8, idx);
    assert_int_equal(ERR_NUMBER_PARSE, d.error);
}

void test_lonely_G(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G ", &idx));
    assert_int_equal(7, idx);
    assert_int_equal(ERR_BAD_GCODE, d.error);
}

void test_lonely_G_cr(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G\n", &idx));
    assert_int_equal(7, idx);
    assert_int_equal(ERR_BAD_GCODE, d.error);
}

void test_unknown_G(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G02 ", &idx));
    assert_int_equal(9, idx);
    assert_int_equal(ERR_BAD_GCODE, d.error);
}

void test_G_too_long(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G111 ", &idx));
    assert_int_equal(9, idx);
    assert_int_equal(ERR_BAD_GCODE, d.error);
}

void test_unknown_token_with_move(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 K1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_unknown_token_with_scale(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G51 K1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_unknown_token_with_zero(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G51 K1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_scale_with_feed(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G51 F1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_zero_with_feed(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G54 F1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_move_with_scale(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 P1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_zero_with_scale(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G54 P1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_scale_with_x(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G51 X1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_scale_with_y(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G51 X1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_scale_with_z(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G51 Z1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_zero_with_z(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G54 Z1\n", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_UNEXPECTED_TOKEN, d.error);
}

void test_lonely_token(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 X ", &idx));
    assert_int_equal(11, idx);
    assert_int_equal(ERR_NUMBER_PARSE, d.error);
}

void test_token_bad_val(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 Xa ", &idx));
    assert_int_equal(12, idx);
    assert_int_equal(ERR_NUMBER_PARSE, d.error);
}

void test_token_bad_negative(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 X1-\n", &idx));
    assert_int_equal(13, idx);
    assert_int_equal(ERR_NUMBER_PARSE, d.error);
}

void test_token_negative_float(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 X1.-1\n", &idx));
    assert_int_equal(15, idx);
    assert_int_equal(ERR_NUMBER_PARSE, d.error);
}

void test_token_too_high(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 X65536\n", &idx));
    assert_int_equal(16, idx);
    assert_int_equal(ERR_NUMBER_TOO_LARGE, d.error);
}

void test_token_too_high_neg(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 X-65536\n", &idx));
    assert_int_equal(17, idx);
    assert_int_equal(ERR_NUMBER_TOO_LARGE, d.error);
}

void test_token_feed_too_low(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 F0.99\n", &idx));
    assert_int_equal(15, idx);
    assert_int_equal(ERR_FEED_RATE_OUT_OF_RANGE, d.error);
}

void test_token_feed_too_high(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(FALSE, feed_data(&d, "%\nN01 G01 F256\n", &idx));
    assert_int_equal(14, idx);
    assert_int_equal(ERR_FEED_RATE_OUT_OF_RANGE, d.error);
}

void test_token_too_long(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(
        FALSE,
        feed_data(
          &d,
          "%\nN01 G01 X00000000000000000",
          &idx));
    assert_int_equal(27, idx);
    assert_int_equal(ERR_NUMBER_TOO_LONG, d.error);
}

void test_token_frac_too_long(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(
        FALSE,
        feed_data(
          &d,
          "%\nN01 G01 X.00000000000000000",
          &idx));
    assert_int_equal(28, idx);
    assert_int_equal(ERR_NUMBER_TOO_LONG, d.error);
}

void test_token_no_leading_zero(void) {
    struct GCodeParseData d;
    int idx = 0;
    gcode_parser_init(&d);
    assert_int_equal(TRUE, feed_data(&d, "%\nN01 G01 X.12\n", &idx));
    assert_int_equal(12, (int)(d.x * 100));
}


void test_token_move_no_scaling(void) {
    struct GCodeParseData d;
    int idx = 0;
    const char* data = (
        ";Testing the parsing of XYZ without scaling\n"
        "%\n"
        "N01 G01 X1234 ; First, just X\n"
        ";Next, do Y\n"
        "N02 G01 Y2.345678\n"
        "\n"
        "N03 G01  Z0 ; Do Z\n"
        "N04 G01 X-1.5 Y0 Z1 F123\n"
        "%\n"
        ";Comments after EOF are fine\n"
    );

    gcode_parser_init(&d);

    assert_int_equal(TRUE, feed_data(&d, data, &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(1, d.line_number);
    assert_int_equal(1234, (int)d.x);
    assert_int_equal(0, (int)d.y);
    assert_int_equal(10, d.feed_rate);
    assert_int_equal(FALSE, d.pen_is_down);

    assert_int_equal(TRUE, feed_data(&d, data, &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(2, d.line_number);
    assert_int_equal(1234, (int)d.x);
    assert_int_equal(23456, (int)(d.y * 10000));
    assert_int_equal(10, d.feed_rate);
    assert_int_equal(FALSE, d.pen_is_down);

    assert_int_equal(TRUE, feed_data(&d, data, &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(3, d.line_number);
    assert_int_equal(1234, (int)d.x);
    assert_int_equal(23456, (int)(d.y * 10000));
    assert_int_equal(10, d.feed_rate);
    assert_int_equal(TRUE, d.pen_is_down);

    assert_int_equal(TRUE, feed_data(&d, data, &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(4, d.line_number);
    assert_int_equal(-15, (int)(d.x * 10));
    assert_int_equal(0, (int)(d.y));
    assert_int_equal(123, d.feed_rate);
    assert_int_equal(FALSE, d.pen_is_down);

    assert_int_equal(FALSE, feed_data(&d, data, &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(TRUE, d.eof);
}

void test_token_move_offset_scale(void) {
    struct GCodeParseData d;
    int idx = 0;
    const char* data = (
        "% ;Testing offset and scale\n"
        "\n"
        "  \t\r\n"
        "; First, Default values\n"
        "  N01 G01\tX2 Y3\t\r\n"
        "; Change offset only\n"
        "N02 G54 X10 Y-10\n"
        "N03 G01 \t X2 Y3\n"
        "; Change scale too\n"
        "N04 G51   P3. ; Missing the number after the dot is OK\n"
        "N05 G01   X2 Y3\n"
        "%\n"
    );

    gcode_parser_init(&d);

    assert_int_equal(TRUE, feed_data(&d, data, &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(1, d.line_number);
    assert_int_equal(2, (int)d.x);
    assert_int_equal(3, (int)d.y);

    assert_int_equal(TRUE, feed_data(&d, data, &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(3, d.line_number);
    assert_int_equal(12, (int)d.x);
    assert_int_equal(-7, (int)d.y);

    assert_int_equal(TRUE, feed_data(&d, data, &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(5, d.line_number);
    assert_int_equal(16, (int)d.x);
    assert_int_equal(-1, (int)d.y);

    assert_int_equal(FALSE, feed_data(&d, data, &idx));
    assert_int_equal(0, d.error);
    assert_int_equal(TRUE, d.eof);
}


int main(void) {
    test(test_error_code_str);
    test(test_no_start);
    test(test_start_only);
    test(test_data_after_eof);
    test(test_trailing_chars);
    test(test_no_N);
    test(test_bad_N);
    test(test_lonely_N);
    test(test_N_bad_parse);
    test(test_N_long);
    test(test_no_G);
    test(test_G_parse);
    test(test_lonely_G);
    test(test_lonely_G_cr);
    test(test_unknown_G);
    test(test_G_too_long);
    test(test_unknown_token_with_move);
    test(test_unknown_token_with_scale);
    test(test_unknown_token_with_zero);
    test(test_zero_with_feed);
    test(test_scale_with_feed);
    test(test_move_with_scale);
    test(test_zero_with_scale);
    test(test_scale_with_x);
    test(test_scale_with_y);
    test(test_scale_with_z);
    test(test_zero_with_z);
    test(test_lonely_token);
    test(test_token_bad_val);
    test(test_token_bad_negative);
    test(test_token_negative_float);
    test(test_token_too_high);
    test(test_token_too_high_neg);
    test(test_token_feed_too_low);
    test(test_token_feed_too_high);
    test(test_token_too_long);
    test(test_token_frac_too_long);
    test(test_token_no_leading_zero);
    test(test_token_move_no_scaling);
    test(test_token_move_offset_scale);

    return 0;
}
