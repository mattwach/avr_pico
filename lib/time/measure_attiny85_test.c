#include "measure.h"

#include <test/unit_test.h>

extern volatile uint32_t _time_measure_counter;

void test_init(void) {
  time_measure_init();

  assert_history(TCCR0A, 0x02);  // no pwm, CTC enabled
  assert_history(TCCR0B, 0x00, 0x00, 0x03); // div 64 prescaler
  assert_history(OCR0A, 250);    // 16ms
  assert_history(TIMSK, 0x10);  // Interrupt on counter match
  assert_history(TCNT0, 0x00);  // Timer reset
  assert_history(SREG, 0x00, 0x80);  // Interrupt disable and enable
}

void test_reset(void) {
  time_measure_reset();

  assert_history(TCCR0B, 0x00, 0x03); // div 64 prescaler
  assert_history(TCNT0, 0x00);  // Timer reset
  assert_history(SREG, 0x00, 0x80);  // Interrupt disable and enable
}

void time_results_helper(uint64_t micros) {
    // round off to the nearest 64
    micros = (micros / 64) * 64;
    // Each counter tick is 16MS
    _time_measure_counter = micros / 16000;
    // We are at 1 Mhz, / 64 so each timer tick is 64 us
    // and 64e-6 * 250 = 16ms
    TCNT0_value = (micros / 64) % 250;

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
    // maximum supported value:
    time_results_helper(34359738352000);

    assert_reg_activity(SREG);
    assert_reg_activity(TCNT0);
}

int main(void) {
    test(test_init);
    test(test_reset);
    test(test_time_results);

    return 0;
}