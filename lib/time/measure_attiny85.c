#include "measure.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#if F_CPU == 8000000
#define CYCLE_8MS
#else
#define CYCLE_16MS
#endif

#define COUNTER_MAX 250

uint32_t _time_measure_counter;

// Note: Interrupts must be disabled when calling this!
static inline void _reset_counter(void) {
    // stop the counter
    TCCR0B = 0x00;
    _time_measure_counter= 0;
    TCNT0 = 0x00;
    // Start the counter
#if F_CPU == 1000000
    TCCR0B = 0x03;  // divide by 64
#elif F_CPU == 8000000
    TCCR0B = 0x04;  // divide by 256
#elif defined(UNIT_TEST)
    // A Hack for the unit test enironment, pretend we are 16ms
    TCCR0B = 0x03;  // divide by 64
#else
#error "F_CPU must be 1000000 or 8000000"
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
    TCCR0B = 0x00;
    // NO PWM, enable CTC on OCRA match
    TCCR0A = (1 << WGM01);

    // Set compare registers
    OCR0A = COUNTER_MAX;

    _reset_counter();

    // Set interrupt masks
    TIMSK = (1 << OCIE0A);

    sei();
}

uint32_t time_measure_secs(void) {
    cli();
#ifdef CYCLE_8MS
    const uint32_t t = _time_measure_counter / 125;
#elif defined(CYCLE_16MS)
    // Note: could also do
    // (t / 62) - (t / 7750)
    // and get the high bit back in trade for extra calculation
    // cycles.
    const uint32_t t = (_time_measure_counter << 1) / 125;
#endif
    sei();
    return t;
} 

uint32_t time_measure_ms(void) {
    cli();
    const uint16_t timer = TCNT0 << 3;
#ifdef CYCLE_8MS
    const uint32_t t =
      _time_measure_counter * 8 + (timer / 250);  // equiv + t * 8 / 250
#elif defined(CYCLE_16MS)
    const uint32_t t =
      _time_measure_counter * 16 + (timer / 125); // equiv + t * 16 / 250 
#endif
    sei();
    return t;
} 

uint32_t time_measure_us(void) {
    cli();
#ifdef CYCLE_8MS
    const uint32_t t =
      _time_measure_counter * 8000 + (TCNT0 << 5); 
#elif defined(CYCLE_16MS)
    const uint32_t t =
      _time_measure_counter * 16000 + (TCNT0 << 6);
#endif
    sei();
    return t;
} 

ISR(TIMER0_COMPA_vect) {
  ++_time_measure_counter;
}
