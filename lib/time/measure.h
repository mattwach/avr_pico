#ifndef TIME_MEASURE_H
#define TIME_MEASURE_H
// Provides a simple way to measure time in seconds, milliseconds, or
// microseconds
//
// Implementation:
//  ATMega:
//    - Uses Timer1 (16-bit).
//    - Resolution is 1us
//    - Interrupts every 25ms @ 16mhz, 50ms @ 8mhz
//  ATTiny85
//    - Uses Timer0 (8-bit)
//    - Resolution is 64us @ 1Mhz and 32us at 8mhz
//    - Interrupts every 16 ms @ 1Mhz, 8ms @ 8 Mhz
//
//  The 8 bit timers make a default implementation tricky to settle on
//  for ATTiny85.  If you need more resolution, you can get it if you
//  use both Timer0 and timer1 and carefully synchronize them.  Or, if you
//  only need to measure short periods of time, you can just use one timer,
//  noting that timer1 can use the internal 64Mhz PLL as a clock source if
//  needed.  Thus a reasoble general solution could be to use this library
//  (Timer0) for measuring longr time periods at 64us resolution and using
//  Timer1 to measure short events with high accuracy.

#include <inttypes.h>

// Initialize and reset timers
void time_measure_init(void);

// Reset time base to zero
void time_measure_reset(void);

// Returns the number of microseconds, milliseconds, and seconds since
// last init or reset
uint32_t time_measure_secs(void);
uint32_t time_measure_ms(void);
uint32_t time_measure_us(void);

#endif