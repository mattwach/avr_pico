// Count up timer, changes once per second assuming the display can
// update that fast.

#include <oledm/epaper.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus16x32.h>
#include <lowpower/lowpower.h>
#include <port/port.h>
#include <pstr/pstr.h>
#include <avr/interrupt.h>

#define OLEDM_INIT oledm_basic_init

struct OLEDM display;
struct Text text;

volatile uint32_t counter;
// Assumes 16Mhz clock
#define ONE_SECOND_COUNT 62500

static void update_counter(void) {
  cli();
  const uint32_t local_counter = counter;
  sei();
  text.row = 3;
  text.column = 7 * 16; // To the left of Count
  epaper_set_partial_mode(&display);
  text_pstr(&text, u32_to_ps(local_counter));
  epaper_update_partial(&display, SLEEP_MODE_1);
}

static void timer_setup(void) {
  OCR1A = ONE_SECOND_COUNT;  // when to fire the interrupt
  TIMSK1 = 1 << OCIE1A; // Interrupt match on counter 1
  TCCR1B = (1 << WGM12) | 0x04;  // OCR1A is top, / 256 count
  sei();  // enable global interrupts
}

void draw_background(void) {
  //display.option_bits |= OLEDM_ROTATE_180;
  //display.option_bits |= OLEDM_WHITE_ON_BLACK;
  oledm_clear(&display, 0x00);
  text.row = 3;
  text.column = 0;
  text_str(&text, "Count: ");
  epaper_swap_buffers(&display, SLEEP_MODE_OFF);
}

int main(void) {
  counter = 0;
  port_set_all_input_pullup();
  oledm_basic_init(&display);
  oledm_start(&display);
  text_init(&text, terminus16x32, &display);

  // This little dance is needed because the partial updates also
  // flip the ram memory
  draw_background();
  display.option_bits |= OLEDM_WRITE_COLOR_RAM;
  draw_background();
  display.option_bits &= ~OLEDM_WRITE_COLOR_RAM;

  timer_setup();
  update_counter();
  while (1) {
    lowpower_idle(
        SLEEP_FOREVER,
        ADC_OFF,
        TIMER2_OFF,
        TIMER1_ON,
        TIMER0_OFF,
        SPI_OFF,
        USART0_OFF,
        TWI_OFF);
    update_counter();
  }
}

ISR(TIMER1_COMPA_vect) {
  ++counter;
}
