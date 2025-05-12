#include "servo_attiny.h"

#include <test/unit_test.h>

extern void TIMER1_COMPA_vect(void);

uint16_t calc_period(uint16_t period_us) {
  period_us -= SERVO_PROCESS_DELAY_US;
  period_us -= period_us / (-SERVO_PROCESS_SCALE);
  return period_us;
}

void test_servo_set(void) {
  servo_stop(1);
  reset_history();
  PORTB_value = 0xFF;
  servo_set(0, 1500);

  assert_history(PORTB, 0xFE);  // clear old, set new one low
  assert_history(DDRB, 0x01); // Make port zero an output
  assert_history(TCCR1, 0x00, 0x08); // Timer off, timer / 128
  assert_history(PLLCSR, 0x00);  // Use sync clock
  assert_history(GTCCR, 0x00);  // No special actions (Maybe Prescaler reset is needed?)
  assert_history(TCNT1, 0x00); // reset count to zero
  assert_history(OCR1A, SERVO_COUNTER_20MS);  // initial delay 
  assert_history(TIMSK, (1 << OCIE1A));  // Interrupt on counter 1 match
  assert_history(SREG, 0x00, 0x80);  // Interrupts off, then on
}

void test_servo_get_initial(void) {
  servo_stop(1);
  reset_history();
  assert_int_equal(0, servo_get_period_us());
}

void test_servo_get(void) {
  servo_stop(1);
  servo_set(1, 1500);
  reset_history();
  assert_int_equal(1500, servo_get_period_us());
}

void test_servo_get_low(void) {
  servo_stop(1);
  servo_set(1, MIN_SERVO_PERIOD_US - 1);
  reset_history();
  assert_int_equal(MIN_SERVO_PERIOD_US, servo_get_period_us());
}

void test_servo_get_high(void) {
  servo_stop(1);
  servo_set(1, MAX_SERVO_PERIOD_US + 1);
  reset_history();
  assert_int_equal(MAX_SERVO_PERIOD_US, servo_get_period_us());
}

void test_servo_stop(void) {
  servo_stop(1);
  PORTB_value = 0xFF;
  TIMSK_value = 0xFF;
  servo_set(1, 1500);
  reset_history();
  servo_stop(0);
  assert_history(PORTB, 0xFD);  // Pin 1 is cleared
  assert_history(TCCR1, 0x00); // Timer off
  assert_history(TIMSK, 0xFF & ~(1 << OCIE1A)); // Interrupt is cleared
  assert_history(SREG, 0x00, 0x80);  // Interrupts off, then on
}

void test_servo_update_same_pin(void) {
  servo_stop(1);
  reset_history();
  PORTB_value = 0xFF;
  servo_set(0, 1500);
  reset_history();
  servo_set(0, 1700);
  assert_history(SREG, 0x00, 0x80);  // Interrupts off, then on
}

void test_servo_update_different_pin(void) {
  servo_stop(1);
  reset_history();
  servo_set(0, 1500);
  reset_history();
  PORTB_value = 0xFF;
  servo_set(1, 1700);
  assert_history(PORTB, 0xFC);  // Drop pin zero and pin one 
  assert_history(DDRB, 0x03);  // Set pin 1 as an output, zero is left as-was
  assert_history(SREG, 0x00, 0x80);  // Interrupts off, then on
}

void test_interrupt_wait_state(void) {
  servo_stop(1);
  reset_history();
  servo_set(0, 1500);
  reset_history();
  TIMER1_COMPA_vect();

  assert_history(TCCR1, 0x00, 0x05);  // Stopped, then div by 16
  assert_history(TCNT1, 0x00);  // Counter is reset
  assert_history(OCR1A, (calc_period(1500) >> 4) - SERVO_FINE_BUFFER);  // 89 cycles (89 * 16 -> 1424 us)
  assert_history(PORTB, 0x01);  // Pin enabled
}

void test_interrupt_wait_state_loop(void) {
  servo_stop(1);
  reset_history();
  servo_set(0, 1500);
  TIMER1_COMPA_vect(); // -> coarse
  TIMER1_COMPA_vect(); // -> fine
  TIMER1_COMPA_vect(); // -> init

  reset_history();
  TIMER1_COMPA_vect();

  assert_history(TCCR1, 0x00, 0x05);  // Stopped, then div by 16
  assert_history(TCNT1, 0x00);  // Counter is reset
  assert_history(OCR1A, (calc_period(1500) >> 4) - SERVO_FINE_BUFFER);  // 89 cycles (89 * 16 -> 1424 us)
  assert_history(PORTB, 0x01);  // Pin enabled
}

void test_interrupt_coarse_state(void) {
  servo_stop(1);
  reset_history();
  servo_set(0, 1500);
  TIMER1_COMPA_vect();
  reset_history();
  TIMER1_COMPA_vect();

  assert_history(TCCR1, 0x00, 0x01);  // Stopped, then no divide
  assert_history(TCNT1, 0x00);  // Counter is reset
  assert_history(OCR1A, (calc_period(1500) & 0x0F) + (SERVO_FINE_BUFFER << 4));  // 76 cycles (76 * 1 -> 76 us + 1424 above = 1500)
}

void test_interrupt_fine_state(void) {
  servo_stop(1);
  reset_history();
  PORTB_value = 0xFF;
  servo_set(0, 1500);
  TIMER1_COMPA_vect();
  TIMER1_COMPA_vect();
  reset_history();
  TIMER1_COMPA_vect();

  assert_history(PORTB, 0xFE);  // Pin disabled
  assert_history(TCCR1, 0x00, 0x08);  // Stopped, then divide by 128
  assert_history(TCNT1, 0x00);  // Counter is reset
  assert_history(OCR1A, SERVO_COUNTER_20MS - (1500 >> 7));  // 50ms wait minus the armed time
}

int main(void) {
  test(test_servo_set);
  test(test_servo_stop);
  test(test_servo_get_initial);
  test(test_servo_get);
  test(test_servo_get_low);
  test(test_servo_get_high);
  test(test_servo_update_different_pin);
  test(test_servo_update_same_pin);
  test(test_interrupt_wait_state);
  test(test_interrupt_wait_state_loop);
  test(test_interrupt_coarse_state);
  test(test_interrupt_fine_state);
  return 0;
}
