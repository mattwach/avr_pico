
#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <oledm/console.h>
#include <oledm/font/terminus6x8.h>
#include <oledm/oledm.h>
#include <oledm/ssd1306_init.h>
#include <uart/uart.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <port/port.h>

#define I2C_ADDRESS 0x40

struct Console console;
struct OLEDM display;

// must be a power of 2
#define BUFFER_SIZE 1024
#define BUFFER_MASK (BUFFER_SIZE - 1)
uint8_t buffer[BUFFER_SIZE];
volatile uint16_t bytes_in_buffer = 0;
volatile uint16_t buffer_head = 0;

static void init() {
  port_set_all_input_pullup();
  _delay_ms(15);
  uart_init(9600, ENABLE_RX | ENABLE_RX_INTERRUPT);
  oledm_basic_init(&display);
  _delay_ms(15);
  oledm_start(&display);

  oledm_clear(&display, 0x00);
  console_init(&console, &display, terminus6x8);

  bytes_in_buffer = 0;
  buffer_head = 0;
  sei();
}

static void dump_buffer_to_console() {
  if (bytes_in_buffer == 0) {
    console_printlen(&console, "[zzz]\n", 6);
  }

  while (1) {
    cli();
    if (bytes_in_buffer == 0) {
      sei();
      return;
    }

    uint8_t byte = buffer[buffer_head];
    buffer_head = (buffer_head + 1) & BUFFER_MASK;
    --bytes_in_buffer;
    sei();
    console_printlen(&console, (const char*)(&byte), 1);
  }
}

int main(void) {
  init();

  while (1) {
    lowpower_idle(
        SLEEP_8S,
	ADC_OFF,
	TIMER2_OFF,
	TIMER1_OFF,
	TIMER0_OFF,
	SPI_OFF,
	USART0_ON,
	TWI_OFF);
    dump_buffer_to_console();
  }
}

ISR(USART_RX_vect) {
  uint8_t byte = 0;
  while (read_no_block(&byte)) {
    if (byte == '\n') {
      continue;
    }
    if (byte == '\r') {
      byte = '\n';
    }
    if (bytes_in_buffer < BUFFER_SIZE) {
      buffer[(buffer_head + bytes_in_buffer) & BUFFER_MASK] = byte;
      ++bytes_in_buffer;
    }
    // else drop the character
  }
}
