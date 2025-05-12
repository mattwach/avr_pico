
#include <lowpower/lowpower.h>
#include <uart/uart.h>
#include <uart/software_uart.h>
#include <pstr/pstr.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

// The HEARTBEAT flashes for 1ms every second and tells us that the
// 32k oscillator is working and the code is not hung up.
#define HEARTBEAT_LED_DDR DDRD
#define HEARTBEAT_LED_PORT PORTD
#define HEARTBEAT_LED_PIN 5

static void heartbeat(void) {
  // Turning on the heartbeat LED for 1ms uses almost no power and due to
  // photoluminescence / persistance of vision, the flash can still be easily
  // detected by the human eye.  Without a "heartbeat" it's really hard to tell
  // if the unit is powered on and working properly.
  HEARTBEAT_LED_PORT |= (1 << HEARTBEAT_LED_PIN);
  _delay_ms(1);
  HEARTBEAT_LED_PORT &= ~(1 << HEARTBEAT_LED_PIN);
}

// For this one, you connect a source to the uart and it will collect stats
// on bytes received and each error count.

#define FIFO_SIZE 64  // must be power of 2
#define FIFO_MASK (FIFO_SIZE - 1)
struct FIFO {
  uint32_t size;
  uint8_t data[FIFO_SIZE];
};

#define SNAPSHOT_COUNTS 8
struct SerialStats {
  uint32_t no_error;
  uint32_t status_counts[SNAPSHOT_COUNTS]; 
  struct FIFO status_fifo;
  struct FIFO error_fifo;
  struct FIFO recv_fifo;
};

volatile struct SerialStats stats;
volatile uint32_t uptime_seconds;
struct SerialStats stats_snapshot;

static inline void fifo_push(volatile struct FIFO* fifo, uint8_t byte) {
  fifo->data[fifo->size & FIFO_MASK] = byte;
  ++fifo->size;
}

#define FIFO_HEX 1
#define FIFO_ASCII 2
static void fifo_dump(const struct FIFO* fifo, const char* name, uint8_t style) {
  uart_str(name);

  const uint8_t head = (fifo->size <= FIFO_SIZE) ? 0 : fifo->size & FIFO_MASK;
  const uint8_t size = (fifo->size <= FIFO_SIZE) ? fifo->size : FIFO_SIZE;

  for (uint8_t i=0; i<size; ++i) {
    const uint8_t c = fifo->data[(head + i) & FIFO_MASK];
    if (style == FIFO_HEX) {
      uart_pstr(u8_to_pshex(c));
      uart_byte(' ');
    } else if (c < 32 || c > 127) {
      uart_byte('(');
      uart_pstr(u8_to_pshex(c));
      uart_byte(')');
    } else {
      uart_byte(c);
    }
  }

  uart_newln();
}


void byte_received(void) {
  const SerialError last_err = software_uart_last_error();
  if (last_err == 0) {
    ++stats.no_error;
  }
  if (last_err > 6) {
    // should not happen
    ++stats.status_counts[7];
  } else {
    ++stats.status_counts[last_err];
  }
  fifo_push(&stats.status_fifo, last_err);
  if (last_err != SERIAL_OK) {
    fifo_push(&stats.error_fifo, last_err);
  }

  uint8_t c;
  if (software_uart_read(&c)) {
    fifo_push(&stats.recv_fifo, c);
  }

  if (software_uart_read(&c)) {
    // should not happen
    ++stats.status_counts[7];
  }
}

#define CHUNK_SIZE 16
void snapshot_stats() {
  uint8_t* src = (uint8_t*)(&stats);
  uint8_t* dest = (uint8_t*)(&stats_snapshot);
  uint16_t i;
  uart_str("debug ");
  cli();
  const uint32_t count = stats.no_error;
  sei();
  uart_pstr(u32_to_ps(count));
  uart_newln();
  for (i=0; i<sizeof(struct SerialStats); i += CHUNK_SIZE) {
    cli();
    memcpy(dest + i, src + i, CHUNK_SIZE);
    sei();
  }

  i -= CHUNK_SIZE;
  if (i < sizeof(struct SerialStats)) {
    cli();
    memcpy(dest + i, src + i, sizeof(struct SerialStats) - i);
    sei();
  }
}

void dump_stats(uint32_t seconds) {
  snapshot_stats();

  uart_str("t: ");
  uart_pstr(u32_to_ps(seconds));
  uart_str(" status: [ ");
  for (uint8_t i=0; i<SNAPSHOT_COUNTS; ++i) {
    uart_pstr(u32_to_ps(stats_snapshot.status_counts[i]));
    uart_byte(' ');
  }
  uart_strln("]");

  fifo_dump(&stats_snapshot.status_fifo, "status_fifo: ", FIFO_HEX);
  fifo_dump(&stats_snapshot.error_fifo, "error_fifo: ", FIFO_HEX);
  fifo_dump(&stats_snapshot.recv_fifo, "recv_fifo: ", FIFO_ASCII);

  uart_newln();
}

static void timer_init(void) {
  OCR1A = (F_CPU >> 8);  // when to fire the interrupt (when using the / 256 prescaler)
  TIMSK1 = 1 << OCIE1A; // Interrupt match on counter 1
  TCCR1B = (1 << WGM12) | 0x04;  // OCR1A is top, / 256 count
}

int main(void) {
  memset((struct SerialStats*)(&stats), 0, sizeof(struct SerialStats));
  uart_init(57600, ENABLE_TX);
  software_uart_init(byte_received);
  HEARTBEAT_LED_DDR |= (1 << HEARTBEAT_LED_PIN);
  timer_init();
  sei();
  uart_strln("Waiting for data");
  uint32_t last_seconds = 0;
  while (1) {
    lowpower_idle(
        SLEEP_FOREVER,
        ADC_OFF,
        TIMER2_OFF,
        TIMER1_ON,
        TIMER0_ON,
        SPI_OFF,
        USART0_ON,
        TWI_OFF);
    cli();
    const uint32_t seconds = uptime_seconds;
    sei();
    if (seconds != last_seconds) {
      heartbeat();
      last_seconds = seconds;
      if ((seconds % 5) == 0) {
        dump_stats(seconds);
      }
    }
  }
}

ISR(TIMER1_COMPA_vect)
{
  // Ah hah!  Ths delay will (now obviously) create timing issues with software uart.
  //heartbeat();
  ++uptime_seconds;
}
