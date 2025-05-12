#if defined(__AVR_MEGA__)
#include "measure_atmega.c"
#elif defined(__AVR_ATtiny85__)
#include "measure_attiny85.c"
#elif defined(__AVR_ATtiny84__)
#include "measure_attiny84.c"
#else
#error Unrecognized CPU Type
#endif
