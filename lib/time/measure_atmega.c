#include "measure.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#if F_CPU == 16000000
#define CYCLE_25MS
#else
#define CYCLE_50MS
#endif

#define COUNTER_MAX 50000

uint32_t _time_measure_counter;


// Note: Interrupts must be disabled when calling this!
static inline void _reset_counter(void) {
    // stop the counter
    TCCR1B = (1 << WGM12);
    _time_measure_counter= 0;
    TCNT1H = 0;
    TCNT1L = 0;
    // Start the counter
#if F_CPU == 1000000
    TCCR1B = (1 << WGM12) | 0x01;  // No division
#elif F_CPU == 8000000
    TCCR1B = (1 << WGM12) | 0x02;  // divide by 8
#elif F_CPU == 16000000
    TCCR1B = (1 << WGM12) | 0x02;  // divide by 8
#else
#error "F_CPU must be 1000000, 8000000, or 16000000"
#endif
}

void time_measure_reset(void) {
    cli();
    _reset_counter();
    sei();
}

void time_measure_init(void) {
    cli();

    // Stop the timer if it's already running
    TCCR1B = (1 << WGM12);
    // NO PWM
    TCCR1A = 0x00;

    // Set compare registers
    OCR1AH = (COUNTER_MAX >> 8); // high bits
    OCR1AL = (COUNTER_MAX & 0xFF); // low bits

    _reset_counter();

    // Set interrupt masks
    TIMSK1 = (1 << OCIE1A);

    sei();
}

uint32_t time_measure_secs(void) {
    cli();
#ifdef CYCLE_25MS
    const uint32_t t = _time_measure_counter / 40;
#elif defined(CYCLE_50MS)
    const uint32_t t = _time_measure_counter / 20;
#endif
    sei();
    return t;
} 

uint32_t time_measure_ms(void) {
    cli();
    // Must read TCNT1L before TCNT1H
    const uint8_t low = TCNT1L;
    const uint8_t high = TCNT1H;
#ifdef CYCLE_25MS
    const uint32_t t =
      _time_measure_counter * 25 +
      (((uint16_t)high << 8) | low) / 2000;
#elif defined(CYCLE_50MS)
    const uint32_t t =
      _time_measure_counter * 50 +
      (((uint16_t)high << 8) | low) / 1000;
#endif
    sei();
    return t;
} 

uint32_t time_measure_us(void) {
    cli();
    // Must read TCNT1L before TCNT1H
    const uint8_t low = TCNT1L;
    const uint8_t high = TCNT1H;
#ifdef CYCLE_25MS
    const uint32_t t =
      _time_measure_counter * 25000 +
      ((((uint16_t)high << 8) | low) >> 1);
#elif defined(CYCLE_50MS)
    const uint32_t t =
      _time_measure_counter * 50000 +
      (((uint16_t)high << 8) | low);
#endif
    sei();
    return t;
} 

ISR(TIMER1_COMPA_vect) {
  ++_time_measure_counter;
}
