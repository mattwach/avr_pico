#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus16x32_upper.h>
#include <pstr/pstr.h>
#include <util/delay.h>
#include <misc/debounce.h>
#include <time/measure_no_interrupt.h>

#include <avr/interrupt.h>

#define OLEDM_INIT oledm_basic_init

// All assume PORTC
#define SW_PIN 0
#define DT_PIN 1
#define CLK_PIN 2

struct OLEDM display;
struct Text text;
struct Debounce debounce;

volatile uint32_t timer_ms;  // for debounce
volatile uint16_t value = 500;
volatile bool_t button_toggle = FALSE;

void update_display(void) {
  text.row = 4;
  text.column = 50;
  if (button_toggle) {
    text_str(&text, "ON ");
  } else {
    text_str(&text, "OFF");
  }
  text.row = 0;
  text.column = 30;
  text_pstr(&text, u16_to_ps(value));
}

int main(void) {
  // Set as inputs
  DDRC &= ~((1 << SW_PIN) | (1 << DT_PIN) | (1 << CLK_PIN));
  // Pullup
  PORTC |= (1 << SW_PIN) | (1 << DT_PIN) | (1 << CLK_PIN);

  // timer setup
  measure_ni_init(MEASURE_DIV_1024);
  measure_ni_reset();

  debounce_init(&debounce, 8);

  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  text_init(&text, terminus16x32_upper, &display);

  PCMSK1 = (1 << SW_PIN) | (1 << CLK_PIN);  // Interrupt on clock pin changes or button presses
  PCICR |= 0x02;  // Enable PCIE1 for pin change interrupts
  sei();

  while (1) {
    update_display();
    _delay_ms(15);
  }
}

ISR(PCINT1_vect) {
  // if SW_PIN is low, then assume its a button press, otherwise assume
  // is a turn
  const uint16_t tdelta_ms = measure_ni_time_ms(0); 
  timer_ms += tdelta_ms;
  measure_ni_reset();
  if (debounce_sample(&debounce, timer_ms, (PINC & (1 << SW_PIN)) == 0) && debounce.val) {
    button_toggle = !button_toggle;
  }

  // checking tdelta_ms helps avoid noisy signals
  if (!debounce.val && tdelta_ms > 5) {
    const bool_t clk_down = (PINC & (1 << CLK_PIN)) == 0;
    const bool_t dt_down = (PINC & (1 << DT_PIN)) == 0;

    if (clk_down) {
      if (dt_down == clk_down) {
        // clockwise
        if (value < 1000) {
          ++value;
        }
      } else {
        // CCW
        if (value > 0) {
          --value;
        }
      }
    }
  }
}
