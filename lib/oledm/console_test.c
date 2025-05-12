#include "console.h"
#include <test/unit_test.h>

uint8_t buff[1024];
int buff_ptr;
struct OLEDM display;
struct Console console;

struct FontASCII my_font = {
    .id = {'F', 'A', 'S', '1'},
    .first_char = 32,
    .last_char = 127,
    .width = 24,
    .height = 2
};

#define TEXT_INIT 0xF0
#define VERIFY_IS_FIXED 0xF1
#define FIXED_STR_END 0xF2
#define VSCROLL 0xF3

//
// Below are fakes of the real dependencies used to reduce the complexity of
// the tests and provide a simple way to assert a log of what happened.
//

void text_init(struct Text* text, const void* font, struct OLEDM* display) {
  text->font = font;
  text->display = display;
  text->row = 0;
  text->column = 0;
  buff_ptr = 0;
  buff[buff_ptr++] = TEXT_INIT;
}

void text_verifyFont(struct Text* text) {
  buff[buff_ptr++] = VERIFY_IS_FIXED;
}

void text_strLen(struct Text* text, const char* str, uint8_t len) {
    memcpy(buff + buff_ptr, str, len);
    buff_ptr += len;
    buff[buff_ptr++] = FIXED_STR_END;
}

void oledm_vscroll(struct OLEDM* display, int8_t rows) {
    display->start_line = (display->start_line + rows) % display->visible_rows;
    buff[buff_ptr++] = VSCROLL;
}

void display_init() {
    display.visible_columns = 128;
    display.visible_rows = 8;
    display.error = 0;
    display.start_line = 0;
}

void test_init(void) {
    display_init();
    console_init(&console, &display, &my_font);
    
    assert_int_equal(1, console.wrap);
    assert_int_equal(5, console.text_width);
    assert_int_equal(2, console.text_height);
    assert_int_equal(0, console.line_pos);

    assert_buff_equal(
        ((uint8_t[]){TEXT_INIT, VERIFY_IS_FIXED}),
        buff,
        buff_ptr);
}

void common_init(void) {
    display_init();
    console_init(&console, &display, &my_font);
    buff_ptr = 0;
}

//
// Test cases start here
//

void test_print_hi(void) {
    common_init();
    console_print(&console, "Hi");

    assert_int_equal(2, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){'H', 'i'}),
        console.line,
        2);
    assert_int_equal(0, buff_ptr);
}

void test_print_cr(void) {
    common_init();
    console_print(&console, "\n");

    assert_int_equal(0, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){VSCROLL, ' ', ' ', ' ', ' ', ' ', FIXED_STR_END}),
        buff,
        buff_ptr);
}

void test_print_cr_cr(void) {
    common_init();
    console_print(&console, "\n\n");

    assert_int_equal(0, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, ' ', ' ', ' ', ' ', ' ', FIXED_STR_END,
            VSCROLL, ' ', ' ', ' ', ' ', ' ', FIXED_STR_END,
        }),
        buff,
        buff_ptr);
}

void test_print_hi_bye(void) {
    common_init();
    console_print(&console, "Hi\nBye");

    assert_int_equal(3, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){'B', 'y', 'e'}),
        console.line,
        3);
    assert_buff_equal(
        ((uint8_t[]){VSCROLL, 'H', 'i', ' ', ' ', ' ', FIXED_STR_END}),
        buff,
        buff_ptr);
}

void test_print_hi_bye_try(void) {
    common_init();
    console_print(&console, "Hi\nBye\nTry");

    assert_int_equal(3, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){'T', 'r', 'y'}),
        console.line,
        3);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, 'H', 'i', ' ', ' ', ' ', FIXED_STR_END,
            VSCROLL, 'B', 'y', 'e', ' ', ' ', FIXED_STR_END
         }),
        buff,
        buff_ptr);
}

void test_print_longline_wrap(void) {
    common_init();
    console_print(&console, "1234567890ab");

    assert_int_equal(2, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){'a', 'b'}),
        console.line,
        2);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, '1', '2', '3', '4', '5', FIXED_STR_END,
            VSCROLL, '6', '7', '8', '9', '0', FIXED_STR_END
         }),
        buff,
        buff_ptr);
}

void test_print_longline_wrap2(void) {
    common_init();
    console_print(&console, "123");
    console_print(&console, "4567890ab");

    assert_int_equal(2, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){'a', 'b'}),
        console.line,
        2);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, '1', '2', '3', '4', '5', FIXED_STR_END,
            VSCROLL, '6', '7', '8', '9', '0', FIXED_STR_END
         }),
        buff,
        buff_ptr);
}

void test_print_longline_wrap_cr(void) {
    common_init();
    console_print(&console, "12345678\n90ab");

    assert_int_equal(4, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){'9', '0', 'a', 'b'}),
        console.line,
        4);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, '1', '2', '3', '4', '5', FIXED_STR_END,
            VSCROLL, '6', '7', '8', ' ', ' ', FIXED_STR_END,
         }),
        buff,
        buff_ptr);
}

void test_print_longline_wrap_cr2(void) {
    common_init();
    console_print(&console, "123");
    console_print(&console, "45678\n90ab");

    assert_int_equal(4, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){'9', '0', 'a', 'b'}),
        console.line,
        4);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, '1', '2', '3', '4', '5', FIXED_STR_END,
            VSCROLL, '6', '7', '8', ' ', ' ', FIXED_STR_END,
         }),
        buff,
        buff_ptr);
}

void test_print_longline_nowrap(void) {
    common_init();
    console.wrap = 0;
    console_print(&console, "1234567890ab");

    assert_int_equal(0, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, '1', '2', '3', '4', '5', FIXED_STR_END,
         }),
        buff,
        buff_ptr);
}

void test_print_longline_nowrap2(void) {
    common_init();
    console.wrap = 0;
    console_print(&console, "123");
    console_print(&console, "4567890ab");

    assert_int_equal(0, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, '1', '2', '3', '4', '5', FIXED_STR_END,
         }),
        buff,
        buff_ptr);
}

void test_print_longline_nowrap_cr(void) {
    common_init();
    console.wrap = 0;
    console_print(&console, "12345678\n90ab");

    assert_int_equal(4, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){'9', '0', 'a', 'b'}),
        console.line,
        4);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, '1', '2', '3', '4', '5', FIXED_STR_END,
         }),
        buff,
        buff_ptr);
}

void test_print_longline_nowrap_cr2(void) {
    common_init();
    console.wrap = 0;
    console_print(&console, "123");
    console_print(&console, "45678\n90ab");

    assert_int_equal(4, console.line_pos);
    assert_buff_equal(
        ((uint8_t[]){'9', '0', 'a', 'b'}),
        console.line,
        4);
    assert_buff_equal(
        ((uint8_t[]){
            VSCROLL, '1', '2', '3', '4', '5', FIXED_STR_END,
         }),
        buff,
        buff_ptr);
}

int main(void) {
    test(test_init);

    test(test_print_hi);
    test(test_print_cr);
    test(test_print_cr_cr);
    test(test_print_hi_bye);
    test(test_print_hi_bye_try);
    test(test_print_longline_wrap);
    test(test_print_longline_wrap2);
    test(test_print_longline_wrap_cr);
    test(test_print_longline_wrap_cr2);
    test(test_print_longline_nowrap);
    test(test_print_longline_nowrap2);
    test(test_print_longline_nowrap_cr);
    test(test_print_longline_nowrap_cr2);

    return 0;
}
