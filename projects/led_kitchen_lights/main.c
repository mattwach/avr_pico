// Implements switch controls for an LED source
//
// Modes:
//   0) OFF - Sleep here if possible
//   1) Full on - Can sleep here too
//   2) Low on - Maybe need PWM here but we can start with ON
//
//       +--------+
// N/C  -|        |- VDD
// N/C  -| Tiny85 |- INT0 (Input toggle switch, tied to ground)
// TEST -|        |- LED High MOSFET gate (with some resistor)
// VSS  -|        |- LED Low MOSFET gate (with some resistor) Reserving TIMER0 for possible PWM
//       +--------+

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <lowpower/lowpower.h>

#include <error_codes.h>

#define INPUT_PIN PB2
#define LED_LOW_PIN PB0
#define LED_HIGH_PIN PB1
//#define TEST_PIN PB4

#define STATE_OFF 0
#define STATE_HIGH 1
#define STATE_LOW 2
#define MAX_STATE STATE_LOW


// Current state of the switch
volatile uint8_t state;

static inline void reset_timer() {
  TCNT1 = 0x00;  // Reset timer to 0
  TIFR |= (1<<TOV1);  // clear the overflow flag
}

static inline bool_t check_timer() {
  // Check for timer overflow (> 0xFF)
  return TIFR & (1 << TOV1);
}

static void advance_state() {
  // Roll though each STATE_*
  ++state;
  if (state > MAX_STATE) {
    state = 0;
  }

  switch (state) {
    case STATE_OFF:
      // Both off
      PORTB &= ~((1 << LED_LOW_PIN) | (1 << LED_HIGH_PIN));
      break;
    case STATE_HIGH:
      // Both on
      PORTB |= (1 << LED_LOW_PIN) | (1 << LED_HIGH_PIN);
      break;
    case STATE_LOW:
      // Note, this code assumes that LED_LOW_PIN is already on
      PORTB &= ~(1 << LED_HIGH_PIN);
      break;
  }
}

int main(void) {
#ifdef TEST_PIN
 // configure the test pin as an output for troubleshooting
  DDRB |= (1 << TEST_PIN);
#endif
  // LED PINS are outputs, everything else as an input
  DDRB = (1 << LED_LOW_PIN) | (1 << LED_HIGH_PIN);
  PORTB |= (1 << INPUT_PIN);  // Enable internal pullup on switch input
  
  // Enable interrupt on switch input
  MCUCR |= (1 << ISC01);  // MCUCR is setting INT0 to trigger on falling edge
  GIMSK |= (1 << INT0);  // ENABLE the INT0 interrupt

  // Setup the timer.  This is for switch debounce.
  // Compare disabled, no PWM, prescaler /1024 (262ms overflow @ 1Mhz)
  TCCR1 = 0x0B;
  // Start at zero (probably not needed)
  TCNT1 = 0x00;

  // Start all interrupts
  sei();
  
  // Nothing to do but wait for an interrupt
  while (1) {
    lowpower_idle(SLEEP_FOREVER, ADC_OFF, TIMER1_ON, TIMER2_OFF); 
  }
}

ISR(INT0_vect) {
  if (!check_timer()) {
    // Timer is not maxed so assume this interrupt is debounce noise.
    return;
  }
  reset_timer();
  advance_state();
}
