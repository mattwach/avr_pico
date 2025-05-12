#ifndef LIB_TIME_MEASURE_NO_INTERRUPT_H
#define LIB_TIME_MEASURE_NO_INTERRUPT_H

#include <inttypes.h>
#include <error_codes.h>

// This library is intended for situations where simple timing
// is needed but using an interrupt is not desired (usually
// because there are other interrupts that are timing-critical)
//
// This is a one-shot timer that has varying resoutions, depending
// on the clock speed of the MCU and the divider.
//
// The timer in use depends on the .c fle selected.
//
// For example, say we are using an atmega328p at 16Mhz and Timer0
// which is an 8-bit timer.  This will need a corresponding .c file
// which we will assume we have.
//
// Here is the basic rundown
//
// MEASURE_DIV_1  Resolution=0.0625 us  Max=15.9375 us
// MEASURE_DIV_8  Resolution=0.5 us  Max=127.5 us
// MEASURE_DIV_64  Resolution=4 us  Max=1.02 ms
// MEASURE_DIV_256  Resolution=16 us  Max=4 ms
// MEASURE_DIV_1024  Resolution=64 us  Max=16 ms
//
// To use the timer, you reset it then measure it.  You'll likely
// need to reset it often, perhaps after every measurement, to
// manage the lack of resolution.
//
// measure_ni_init(MEASURE_DIV_1024); // Only need to do this once
// ...
// measure_ni_reset();  // call this before each measurement
// ...
// uint16_t time_us = measure_ni_time_us(NULL); // This will max out at 16320
// // If we care about max, pass in a bool_t
// bool_t is_max;
// time_us = measure_ni_time_us(&is_max);

#define MEASURE_DIV_1 1
#define MEASURE_DIV_8 2
#define MEASURE_DIV_64 3
#define MEASURE_DIV_256 4
#define MEASURE_DIV_1024 5

void measure_ni_init(uint8_t divider);

void measure_ni_reset(void);

uint16_t measure_ni_time_us(bool_t* is_max);
uint16_t measure_ni_time_ms(bool_t* is_max);

#endif

