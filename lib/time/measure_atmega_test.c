#include "measure.h"

#include <test/unit_test.h>

extern volatile uint32_t _time_measure_counter;

void test_init(void) {
  time_measure_init();

  assert_history(TCCR1A, 0x00);  // no pwm
  assert_history(TCCR1B, 0x08, 0x08, 0x0A); // div 8 prescaler
  assert_history(OCR1AH, 0xC3);  // 50ms high bits
  assert_history(OCR1AL, 0x50);  // 50ms low bits
  assert_history(TIMSK1, 0x02);  // Interrupt on counter match
  assert_history(TCNT1H, 0x00);  // Timer reset
  assert_history(TCNT1L, 0x00);  // Timer reset
  assert_history(SREG, 0x00, 0x80);  // Interrupt disable and enable
}

void test_reset(void) {
  time_measure_reset();

  assert_history(TCCR1B, 0x08, 0x0A); // div 8 prescaler
  assert_history(TCNT1H, 0x00);  // Timer reset
  assert_history(TCNT1L, 0x00);  // Timer reset
  assert_history(SREG, 0x00, 0x80);  // Interrupt disable and enable
}

void time_results_helper(uint64_t micros) {
    // * 2 below is because F_CPU == 16000000
    _time_measure_counter = (micros * 2) / 50000;
    uint16_t low_bits = (micros * 2) % 50000;
    TCNT1H_value = low_bits >> 8;
    TCNT1L_value = low_bits & 0xFF;

    assert_int_equal(micros, time_measure_us());
    assert_int_equal(micros / 1000, time_measure_ms());
    assert_int_equal(micros / 1000000, time_measure_secs());
}

void test_time_results(void) {
    time_results_helper(0);
    time_results_helper(1);
    time_results_helper(12);
    time_results_helper(123);
    time_results_helper(1234);
    time_results_helper(12345);
    time_results_helper(123456);
    time_results_helper(1234567);
    time_results_helper(12345678);
    time_results_helper(123456789);
    time_results_helper(1234567890);
    time_results_helper(12345678901);
    time_results_helper(123456789012);
    time_results_helper(1234567890123);
    time_results_helper(12345678901234);
    // maximum supported values:
    //
    // 3 years, 147 days, 18 hours, 9 minutes, 42 seconds @ 16mhz
    // (double that at 1 and 8 mhz)
    time_results_helper(107374182375000);

    assert_reg_activity(SREG);
    assert_reg_activity(TCNT1L);
    assert_reg_activity(TCNT1H);
}

int main(void) {
    test(test_init);
    test(test_reset);
    test(test_time_results);

    return 0;
}