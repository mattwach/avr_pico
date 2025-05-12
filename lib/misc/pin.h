#ifndef LIB_MISC_PIN_H
#define LIB_MISC_PIN_H

#include <avr/io.h>


// Generic pin access functions.
//
// Performance-wise, it's better to directly access the port
//
// PORTB |= 1 << PINB  // or just = if you dont care about setting multiple pins
//
// But this creates an API issue.  How does an API say which pin to use then different
// pins use different ports?
//
// The solution is a wrapper function, similar to Ardiuno's digitalWrite.  It will cost
// performance cycles, but will allow for sequentially-numbered pins.

#ifdef __LINUX__
// This is unfortunately needed because gcc is giving
// warning: left shift count is negative even though
// there is a conditional that guards against this.
#pragma GCC diagnostic ignored "-Wshift-count-negative"
#endif

#ifdef __AVR_MEGA__

#define set_pin(pin) \
  if ((pin) < 8) \
    PORTD |= 1 << (pin); \
  else \
    PORTB |= 1 << ((pin) - 8);

#define clr_pin(pin) \
  if ((pin) < 8) \
    PORTD &= ~(1 << (pin)); \
  else \
    PORTB &= ~(1 << ((pin) - 8));

#define set_pin_as_output(pin) \
  if ((pin) < 8) \
    DDRD |= 1 << (pin); \
  else \
    DDRB |= 1 << ((pin) - 8);

#define set_pin_as_input(pin) \
  if ((pin) < 8) \
    DDRD &= ~(1 << (pin)); \
  else \
    DDRB &= ~(1 << ((pin) - 8));

#else  // __AVR_MEGA__

#define set_pin(pin) PORTB |= 1 << (pin)

#define clr_pin(pin) PORTB &= ~(1 << (pin))

#define set_pin_as_output(pin) DDRB |= 1 << (pin)

#define set_pin_as_input(pin) PORTB &= ~(1 << (pin))

#endif  // __AVR_MEGA__

#endif  // LIB_MISC_PIN_H
