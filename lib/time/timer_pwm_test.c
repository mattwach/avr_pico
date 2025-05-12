#include "timer_pwm.h"

#include <test/unit_test.h>

void test_timer_pwm_us_to_cycles(void) {
  // looking for 8000000 cycles
  assert_int_equal(8, timer_pwm_us_to_cycles(1));
  assert_int_equal(8000000, timer_pwm_us_to_cycles(1000000));
}

void test_timer_pwm_hz_to_cycles(void) {
  // looking for 8000000 cycles
  assert_int_equal(8, timer_pwm_hz_to_cycles(1000000));
  assert_int_equal(8000000, timer_pwm_hz_to_cycles(1));
}

void test_timer_pwm_16bit_50pct_1us(void) {
  timer_pwm_16bit_50pct(timer_pwm_us_to_cycles(1));
  // expect cs=1 and 8 cycles
  assert_reg_equal(0x43, TCCR1A);
  assert_reg_equal(0x19, TCCR1B);
  assert_reg_equal(0x08, OCR1A);
}

void test_timer_pwm_16bit_50pct_10000us(void) {
  timer_pwm_16bit_50pct(timer_pwm_us_to_cycles(10000));
  // In this case, we would have 80,000 cycles which is too
  // many, thus we need to use the / 8 counter.
  // This will leave the count at 10000 and the cs at 2
  assert_reg_equal(0x43, TCCR1A);
  assert_reg_equal(0x1A, TCCR1B);
  assert_reg_equal(10000 & 0xFF, OCR1A);  // can only compare bottom
}

void test_timer_pwm_16bit_50pct_100000us(void) {
  timer_pwm_16bit_50pct(timer_pwm_us_to_cycles(100000));
  // In this case, we would have 800,000 cycles which is too
  // many, thus we need to use the / 64 counter.
  // This will leave the count at 12500 and the cs at 3
  assert_reg_equal(0x43, TCCR1A);
  assert_reg_equal(0x1B, TCCR1B);
  assert_reg_equal(12500 & 0xFF, OCR1A);  // can only compare bottom
}

void test_timer_pwm_16bit_50pct_1000000us(void) {
  timer_pwm_16bit_50pct(timer_pwm_us_to_cycles(1000000));
  // In this case, we would have 8,000,000 cycles which is too
  // many, thus we need to use the / 256 counter.
  // This will leave the count at 31250 and the cs at 4
  assert_reg_equal(0x43, TCCR1A);
  assert_reg_equal(0x1C, TCCR1B);
  assert_reg_equal(31250 & 0xFF, OCR1A);  // can only compare bottom
}

void test_timer_pwm_16bit_50pct_4000000us(void) {
  timer_pwm_16bit_50pct(timer_pwm_us_to_cycles(4000000));
  // In this case, we would have 32,000,000 cycles which is too
  // many, thus we need to use the / 1024 counter.
  // This will leave the count at 31250 and the cs at 5
  assert_reg_equal(0x43, TCCR1A);
  assert_reg_equal(0x1D, TCCR1B);
  assert_reg_equal(31250 & 0xFF, OCR1A);  // can only compare bottom
}

int main(void) {
    test(test_timer_pwm_us_to_cycles);
    test(test_timer_pwm_hz_to_cycles);
    test(test_timer_pwm_16bit_50pct_1us);
    test(test_timer_pwm_16bit_50pct_10000us);
    test(test_timer_pwm_16bit_50pct_100000us);
    test(test_timer_pwm_16bit_50pct_1000000us);
    test(test_timer_pwm_16bit_50pct_4000000us);

    return 0;
}