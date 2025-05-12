#include "servo_atmega.h"

#include <test/unit_test.h>

extern void TIMER1_COMPA_vect(void);

void test_servo_action(void) {
    servo_init_slot(0, 1, 0, 0, 1000);  // PB0
    assert_history(DDRB, 0x01);
    assert_history(DDRC, 0x00);
    assert_history(DDRD, 0x00);
    assert_history(SREG, 0x00, 0x80);
    reset_history();

    servo_init_slot(1, 0, 1 << 1, 0, 1500);  // PC1
    assert_history(DDRB, 0x01);
    assert_history(DDRC, 0x02);
    assert_history(DDRD, 0x00);
    assert_history(SREG, 0x00, 0x80);
    reset_history();

    servo_init_slot(2, 0, 0, 1 << 2, 2000);  // PD2
    assert_history(DDRB, 0x01);
    assert_history(DDRC, 0x02);
    assert_history(DDRD, 0x04);
    assert_history(SREG, 0x00, 0x80);
    reset_history();

	assert_int_equal(1000, servo_get_period_us(0));
	assert_int_equal(1500, servo_get_period_us(1));
	assert_int_equal(2000, servo_get_period_us(2));

    servo_set_count(3);
    assert_history(TCCR1A, 0x00);
    assert_history(TCCR1B, 0x00, 0x01);
    assert_history(TCNT1, 0x0000);
    assert_history(TIMSK1, 1 << OCIE1A);
    assert_history(OCR1A, (uint8_t)(1000 * 16 - DELAY_CYCLES));
    assert_history(PORTB, 0x01);
    assert_history(PORTC, 0x00);
    assert_history(PORTD, 0x00);
    assert_history(SREG, 0x80);
    reset_history();
    
    // End of Servo 0's pulse, start servo 1's pulse
    TIMER1_COMPA_vect();
    assert_history(TCCR1B, 0x00, 0x01);
    assert_history(TCNT1, 0x0000);
    assert_history(PORTB, 0x00, 0x00);
    assert_history(PORTC, 0x00, 0x02);
    assert_history(PORTD, 0x00, 0x00);
    assert_history(OCR1A, (uint8_t)(1500 * 16 - DELAY_CYCLES));
    reset_history();

    // End of Servo 1's pulse, start servo 2's pulse
    TIMER1_COMPA_vect();
    assert_history(TCCR1B, 0x00, 0x01);
    assert_history(TCNT1, 0x0000);
    assert_history(PORTB, 0x00, 0x00);
    assert_history(PORTC, 0x00, 0x00);
    assert_history(PORTD, 0x00, 0x04);
    assert_history(OCR1A, (uint8_t)(2000 * 16 - DELAY_CYCLES));
    reset_history();

    // End of Servo 2's pulse, start wait
    TIMER1_COMPA_vect();
    assert_history(TCCR1B, 0x00, 0x02);
    assert_history(TCNT1, 0x0000);
    assert_history(PORTB, 0x00);
    assert_history(PORTC, 0x00);
    assert_history(PORTD, 0x00);
    assert_history(OCR1A, (uint8_t)((10000 - 4500) * 2));
    reset_history();

    // End of wait, start servo 0's pulse again
    TIMER1_COMPA_vect();
    assert_history(TCCR1B, 0x00, 0x01);
    assert_history(TCNT1, 0x0000);
    assert_history(PORTB, 0x01);
    assert_history(PORTC, 0x00);
    assert_history(PORTD, 0x00);
    assert_history(OCR1A, (uint8_t)(1000 * 16 - DELAY_CYCLES));
    reset_history();

    // Now set the servo count to zero and make sure things shut down
    servo_set_count(0);
    assert_history(TCCR1B, 0x00);
    assert_history(TIMSK1, 0x00);
    assert_history(PORTB, 0x00, 0x00, 0x00);
    assert_history(PORTC, 0x00, 0x00, 0x00);
    assert_history(PORTD, 0x00, 0x00, 0x00);
}

int main(void) {
    test(test_servo_action);
    return 0;
}
