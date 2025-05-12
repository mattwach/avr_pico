// NRF2401 test receiver

#include <avr/interrupt.h>
#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <oledm/console.h>
#include <oledm/font/terminus6x8.h>
#include <oledm/oledm.h>
#include <oledm/ssd1306_init.h>
#include <pstr/pstr.h>
#include <rf24/rf24.h>

#define RADIO_IRQ     2
#define TIMEOUT_MS 120000  // 2 minutes
#define min(X,Y) ((X)<(Y) ? (X) : (Y))

#define OLEDM_INIT oledm_basic_init

struct Console console;
struct OLEDM display;
RF24 radio;
volatile uint32_t sleep_timeout_ms;
uint16_t msg_number;

// Radio address.  Transmitter and receiver need to match
// the \0 at the end is just for printing.  The rf24 does not use it.
//const uint8_t address[7] = "Wach0W\0";
const uint8_t address[7] = "LIDDIE\0";
// Storage for getting data
#define RECV_BUFF_SIZE 512
uint8_t recv_buff[RECV_BUFF_SIZE];
volatile uint8_t recv_buff_offset;

// Interrupt routine called with the radio has data
// to give us
ISR(INT0_vect) {
  sleep_timeout_ms = 0;
  while (rf24_available(&radio)) {
    const uint8_t bytes_read = rf24_get_dynamic_payload_size(&radio);
    if (bytes_read < 1) {
      // corrupted payload
      continue;
    }
    rf24_read(
      &radio,
      recv_buff + recv_buff_offset, 
      min(
          bytes_read,
          RECV_BUFF_SIZE - 1 -recv_buff_offset   // - 1 -> space for the '\0'
      )
    );
    recv_buff_offset += bytes_read;
  }
}


void init_oled(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  console_init(&console, &display, terminus6x8);
  console_print(&console, "Listening at address ");
  console_print(&console, (const char*)address);
  console_print(&console, "\n");
}


void fatal(const char* msg) {
  console_print(&console, msg);
  console_print(&console, "\n");
  while (1) {
    lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}


bool_t init_radio(void) {
  DDRD &= ~(1 << RADIO_IRQ);  // Set IRQ as an input
  // maybe not needed
  //PORTD = (1 << RADIO_IRQ); // Enable pullup
  rf24_init(&radio, 9, 10); // CE, CSN
  if (!rf24_begin(&radio)) {
    return FALSE;
  }
  rf24_enable_dynamic_payloads(&radio);
  rf24_set_data_rate(&radio, RF24_250KBPS);
  rf24_set_pa_level(&radio, RF24_PA_HIGH);
  rf24_open_reading_pipe(&radio, 0, address);
  rf24_start_listening(&radio);

  // Enable INT0 on the falling edge
  cli();
  EICRA |= (1 << ISC01);
  EICRA &= ~(1 << ISC00);
  EIMSK |= (1 << INT0);
  sei();
  return TRUE;
}


void print(const char* msg) {
  ++msg_number;
  console_pprint(&console, u16_to_ps(msg_number));
  console_print(&console, " | ");
  console_print(&console, msg);
  console_print(&console, "\n");
}


void flush_recv_buffer(void) {
  cli();
  recv_buff[recv_buff_offset] = '\0';
  print((const char*)recv_buff);
  recv_buff_offset = 0;
  sei();
}


void loop(void) {
  while (TRUE) {
    for (sleep_timeout_ms = 0; sleep_timeout_ms < TIMEOUT_MS; sleep_timeout_ms+=4000) {
      lowpower_powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
      if (recv_buff_offset > 0) {
        flush_recv_buffer();
      }
    }
    cli();
    print("TIMEOUT");
    sei();
  }
}


int main(void) {
  msg_number = 0;
  recv_buff_offset = 0;
  sei();
  init_oled();
  init_radio();
  loop();
}

