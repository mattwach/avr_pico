#include "measure_no_interrupt.h"

#include <test/unit_test.h>
#include <stdio.h>

//extern volatile uint32_t _time_measure_counter;

void test_init(void) {
  measure_ni_init(MEASURE_DIV_1024);

  assert_history(TCCR0A, 0x00);  // Vanilla setup
  assert_history(TIMSK0, 0x00);  // Interrupt on counter match
  assert_history(TCCR0B, 0x05);  // Set clock to / 1024
}

void test_reset(void) {
  measure_ni_reset();

  assert_history(TCNT0, 0x00);  // Timer reset
  assert_history(TIFR0, 0x01);  // TOV0 reset
}

void measure_no_overflow_helper(uint8_t div_setting, uint32_t div) {
  printf("div_setting=%d, div=%d\n", div_setting, div);
  measure_ni_init(div_setting);

  TCNT0_value = 200;

  const uint32_t micros = TCNT0_value * div / 16;  // 16Mhz
  bool_t max_int = 0;

  assert_int_equal(micros, measure_ni_time_us(&max_int));
  assert_int_equal(0, max_int);

  assert_int_equal(micros / 1000, measure_ni_time_ms(&max_int));
  assert_int_equal(0, max_int);

  reset_history();
}

void test_measure_no_overflow(void) {
  measure_no_overflow_helper(MEASURE_DIV_1, 1);
  measure_no_overflow_helper(MEASURE_DIV_8, 8);
  measure_no_overflow_helper(MEASURE_DIV_64, 64);
  measure_no_overflow_helper(MEASURE_DIV_256, 256);
  measure_no_overflow_helper(MEASURE_DIV_1024, 1024);
}

void measure_with_overflow_helper(uint8_t div_setting, uint32_t div) {
  measure_ni_init(MEASURE_DIV_64);

  TCNT0_value = 200;
  TIFR0_value = 0x01;

  const uint32_t micros = 255 * 64 / 16;  // 16Mhz
  bool_t max_int = 0;

  assert_int_equal(micros, measure_ni_time_us(&max_int));
  assert_int_equal(1, max_int);

  max_int = 0;
  assert_int_equal(micros / 1000, measure_ni_time_ms(&max_int));
  assert_int_equal(1, max_int);

  reset_history();
}

void test_measure_with_overflow(void) {
  measure_with_overflow_helper(MEASURE_DIV_1, 1);
  measure_with_overflow_helper(MEASURE_DIV_8, 8);
  measure_with_overflow_helper(MEASURE_DIV_64, 64);
  measure_with_overflow_helper(MEASURE_DIV_256, 256);
  measure_with_overflow_helper(MEASURE_DIV_1024, 1024);
}

int main(void) {
    test(test_init);
    test(test_reset);
    test(test_measure_no_overflow);
    test(test_measure_with_overflow);

    return 0;
}
