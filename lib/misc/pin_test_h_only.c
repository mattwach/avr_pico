// Lets test the MEGA path because it has more logic
#define __AVR_MEGA__
#include "pin.h"

#include <test/unit_test.h>

#pragma GCC diagnostic ignored "-Wshift-count-negative"

void test_set_zero(void) {
  set_pin(0);
  assert_reg_equal(0x01, PORTD);
}

void test_set_seven(void) {
  set_pin(7);
  assert_reg_equal(0x80, PORTD);
}

void test_set_eight(void) {
  set_pin(8);
  assert_reg_equal(0x01, PORTB);
}

void test_set_thirteen(void) {
  set_pin(13);
  assert_reg_equal(0x20, PORTB);
}

void test_clear_zero(void) {
  PORTD = 0xFF;
  clr_pin(0);
  assert_reg_equal(0xFE, PORTD);
}

void test_clear_seven(void) {
  PORTD = 0xFF;
  clr_pin(7);
  assert_reg_equal(0x7F, PORTD);
}

void test_clear_eight(void) {
  PORTB = 0xFF;
  clr_pin(8);
  assert_reg_equal(0xFE, PORTB);
}

void test_clear_thirteen(void) {
  PORTB = 0xFF;
  clr_pin(13);
  assert_reg_equal(0xDF, PORTB);
}

void test_set_as_output_zero(void) {
  set_pin_as_output(0);
  assert_reg_equal(0x01, DDRD);
}

void test_set_as_output_seven(void) {
  set_pin_as_output(7);
  assert_reg_equal(0x80, DDRD);
}

void test_set_as_output_eight(void) {
  set_pin_as_output(8);
  assert_reg_equal(0x01, DDRB);
}

void test_set_as_output_thirteen(void) {
  set_pin_as_output(13);
  assert_reg_equal(0x20, DDRB);
}

void test_set_as_input_zero(void) {
  DDRD = 0xFF;
  set_pin_as_input(0);
  assert_reg_equal(0xFE, DDRD);
}

void test_set_as_input_seven(void) {
  DDRD = 0xFF;
  set_pin_as_input(7);
  assert_reg_equal(0x7F, DDRD);
}

void test_set_as_input_eight(void) {
  DDRB = 0xFF;
  set_pin_as_input(8);
  assert_reg_equal(0xFE, DDRB);
}

void test_set_as_input_thirteen(void) {
  DDRB = 0xFF;
  set_pin_as_input(13);
  assert_reg_equal(0xDF, DDRB);
}

int main(void) {
  test(test_set_zero);
  test(test_set_seven);
  test(test_set_eight);
  test(test_set_thirteen);
  test(test_clear_zero);
  test(test_clear_seven);
  test(test_clear_eight);
  test(test_clear_thirteen);
  test(test_set_as_output_zero);
  test(test_set_as_output_seven);
  test(test_set_as_output_eight);
  test(test_set_as_output_thirteen);
  test(test_set_as_input_zero);
  test(test_set_as_input_seven);
  test(test_set_as_input_eight);
  test(test_set_as_input_thirteen);
  return 0;
}
