#include "rotary_encoder.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include <misc/debounce.h>
#include <time/measure_no_interrupt.h>

// All assume PORTC
#define SW_PIN 0
#define DT_PIN 1
#define CLK_PIN 2

static struct Debounce g_debounce;
static volatile uint32_t g_timer_ms;  // for debounce
static volatile uint16_t g_value;
static volatile bool_t g_button_toggle;
static uint16_t g_min_value;
static uint16_t g_max_value;

// Initialize the rotary encoder
void rotary_init(uint16_t min_val, uint16_t max_val, uint16_t val) {
  g_button_toggle = FALSE;
  rotary_set(min_val, max_val, val);

  // Set as inputs
  DDRC &= ~((1 << SW_PIN) | (1 << DT_PIN) | (1 << CLK_PIN));
  // Pullup
  PORTC |= (1 << SW_PIN) | (1 << DT_PIN) | (1 << CLK_PIN);
  // Set up interrupt masks, in case they are enabled
  PCICR &= ~0x02;  // Disabled by default
  PCMSK1 = (1 << SW_PIN) | (1 << CLK_PIN);  // Interrupt on clock pin changes or button presses

  // timer setup
  measure_ni_init(MEASURE_DIV_1024);
  measure_ni_reset();

  debounce_init(&g_debounce, 8);
}

// Change the rotary encoder limits and current value
void rotary_set(uint16_t min_val, uint16_t max_val, uint16_t val) {
  if (min_val > max_val) {
    min_val = max_val;
  }
  if (val < min_val) {
    val = min_val;
  }
  if (val > max_val) {
    val = max_val;
  }
  g_min_value = min_val;
  g_max_value = max_val;
  g_value = val;
}

bool_t rotary_button_toggle(void) {
  return g_button_toggle;
}

bool_t rotary_button_pressed(void) {
  return (PINC & (1 << SW_PIN)) == 0;
}

uint16_t rotary_value(void) {
  return g_value;
}

static void edge_triggered(uint8_t pinc) {
  // if SW_PIN is low, then assume its a button press, otherwise assume
  // is a turn
  const uint16_t tdelta_ms = measure_ni_time_ms(0); 
  g_timer_ms += tdelta_ms;
  measure_ni_reset();
  if (debounce_sample(&g_debounce, g_timer_ms, (pinc & (1 << SW_PIN)) == 0) && g_debounce.val) {
    g_button_toggle = !g_button_toggle;
  }

  // checking tdelta_ms helps avoid noisy signals
  if (!g_debounce.val && tdelta_ms > 5) {
    const bool_t clk_down = (pinc & (1 << CLK_PIN)) == 0;
    const bool_t dt_down = (pinc & (1 << DT_PIN)) == 0;

    if (clk_down) {
      if (dt_down == clk_down) {
        // clockwise
        if (g_value >= g_max_value) {
          g_value = g_max_value;
        } else {
          ++g_value;
        }
      } else {
        // CCW
        if (g_value <= g_min_value) {
          g_value = g_min_value;
        } else {
          --g_value;
        }
      }
    }
  }
}

void rotary_poll_ms(uint16_t ms) {
  uint8_t pin_trigger = PINC & ((1 << SW_PIN) | (1 << CLK_PIN));

  for (uint8_t i=0; i < ms; ++i) {
    for (uint8_t j=0; j < 10; ++j) {
      const uint8_t pinc = PINC & ((1 << SW_PIN) | (1 << DT_PIN) | (1 << CLK_PIN));
      const uint8_t new_pin_trigger = pinc & ((1 << SW_PIN) | (1 << CLK_PIN));
      // Ignore DT on comparisons because we don't trigger on that edge
      if (pin_trigger != new_pin_trigger) {
        pin_trigger = new_pin_trigger;
        edge_triggered(pinc);
      }
      _delay_us(100);
    }
  } 
}

void rotary_use_interrupts(bool_t use) {
  if (use) {
    PCICR |= 0x02;
  } else {
    PCICR &= ~0x02;
  }
}

ISR(PCINT1_vect) {
  edge_triggered(PINC);
}
